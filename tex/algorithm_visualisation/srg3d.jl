using DataStructures;
using Makie
using AbstractPlotting, GLMakie

struct Coord
  x::Int32
  y::Int32
  z::Int32
end
struct LapCoord
  coord
  lap::UInt64
end
function data_pos(sizes::Coord, offset::Int32)::Vec3{Int32}
  coffset::Int32 = offset
  z = div(coffset,sizes.x*sizes.y)
  coffset -= z*sizes.x*sizes.y
  y = div(coffset,sizes.x)
  coffset -= y*sizes.x
  x = coffset
  return Vec3{Int32}(x,y,z)
end
function data_offset(sizes::Coord, pos::Coord)::Int32
  return pos.x + pos.y*sizes.x + pos.z*sizes.x*sizes.y + 1; #+1 due to Julia counting from 1 instead of 0
end
function check_enqueue_vox(sizes::Coord, data::Array{UInt8}, queue::Queue{LapCoord}, vpos::Coord, lap::UInt64)
  offset::Int32 = data_offset(sizes, vpos)
  if (data[offset] == 1)
    data[offset] = 255
    #println(vpos)
    #println("offset:",data[offset])
    enqueue!(queue, LapCoord(vpos,lap))
  end
end
#45 rounds?
#srg(step,sizes,data,queue,45)
function srg(step::UInt64, sizes::Coord, data::Array{UInt8}, queue::Queue{LapCoord}, lastround::UInt64)
  while (isempty(queue) == false && first(queue).lap <= lastround) #FIFO has wrong command
    #println("step:",step," qsize:",length(queue))
    elem = dequeue!(queue)
    pos::Coord = elem.coord
    lap = elem.lap
    #println("lap:",lap)
    step += 1
    if (pos.x+1 < sizes.x)
      vpos = Coord(pos.x+1,pos.y,pos.z)
      check_enqueue_vox(sizes, data, queue, vpos, elem.lap+1)
    end
    if (pos.x-1 >= 0)
      vpos = Coord(pos.x-1,pos.y,pos.z)
      check_enqueue_vox(sizes, data, queue, vpos, elem.lap+1)
    end
    if (pos.y+1 < sizes.y)
      vpos = Coord(pos.x,pos.y+1,pos.z)
      check_enqueue_vox(sizes, data, queue, vpos, elem.lap+1)
    end
    if (pos.y-1 >= 0)
      vpos = Coord(pos.x,pos.y-1,pos.z)
      check_enqueue_vox(sizes, data, queue, vpos, elem.lap+1)
    end
    if (pos.z+1 < sizes.z)
      vpos = Coord(pos.x,pos.y,pos.z+1)
      check_enqueue_vox(sizes, data, queue, vpos, elem.lap+1)
    end
    if (pos.z-1 >= 0)
      vpos = Coord(pos.x,pos.y,pos.z-1)
      check_enqueue_vox(sizes, data, queue, vpos, elem.lap+1)
    end
  end
end
function computePlotSizes(size::Coord)::Coord
  return Coord(2*size.x + 1, 2*size.y + 1, 2*size.z + 1)
end
function transform_into_plotcoord(pos::Vec3{Int32})::Vec3{Int32}
  #return Vec3{Int32}(pos[1]*2+1,pos[2]*2+1,pos[3]*2+1)
  return Vec3{Int32}(pos[1]+1,pos[2]+1,pos[3]+1)
end
function Cube_surface_edges(vecmin::Vec3{Int32}, vecdist::Vec3{Int32}, outx::Array{Int32,1}, outy::Array{Int32,1}, outz::Array{Int32,1})
  vecmax::Vec3 = vecmin + vecdist
  #x000-x001-x011-x010 \\
  #x000-x100-x101-x001 \\
  #x000-x010-x110-x111 \\
  #x011-x111-x101-x100 \\
  #x110
  #
  #x000
  append!(outx, vecmin[1])
  append!(outy, vecmin[2])
  append!(outz, vecmin[3]) #
  append!(outx, vecmin[1])
  append!(outy, vecmin[2])
  append!(outz, vecmax[3]) #
  append!(outx, vecmin[1])
  append!(outy, vecmax[2])
  append!(outz, vecmax[3]) #
  append!(outx, vecmin[1])
  append!(outy, vecmax[2])
  append!(outz, vecmin[3]) #
  # x000
  append!(outx, vecmin[1])
  append!(outy, vecmin[2])
  append!(outz, vecmin[3]) #
  append!(outx, vecmax[1])
  append!(outy, vecmin[2])
  append!(outz, vecmin[3]) #
  append!(outx, vecmax[1])
  append!(outy, vecmin[2])
  append!(outz, vecmax[3]) #
  append!(outx, vecmin[1])
  append!(outy, vecmin[2])
  append!(outz, vecmax[3]) #
  # x000
  append!(outx, vecmin[1])
  append!(outy, vecmin[2])
  append!(outz, vecmin[3]) #
  append!(outx, vecmin[1])
  append!(outy, vecmax[2])
  append!(outz, vecmin[3]) #
  append!(outx, vecmax[1])
  append!(outy, vecmax[2])
  append!(outz, vecmin[3]) #
  append!(outx, vecmax[1])
  append!(outy, vecmax[2])
  append!(outz, vecmax[3]) #
  # x011
  append!(outx, vecmin[1])
  append!(outy, vecmax[2])
  append!(outz, vecmax[3]) #
  append!(outx, vecmax[1])
  append!(outy, vecmax[2])
  append!(outz, vecmax[3]) #
  append!(outx, vecmax[1])
  append!(outy, vecmin[2])
  append!(outz, vecmax[3]) #
  append!(outx, vecmax[1])
  append!(outy, vecmin[2])
  append!(outz, vecmin[3]) #
  # x110
  append!(outx, vecmax[1])
  append!(outy, vecmax[2])
  append!(outz, vecmin[3]) #
end
function plotVoxel(posmin::Vec3{Int32}, starting::Bool)
  xout::Array{Int32,1} = []
  yout::Array{Int32,1} = []
  zout::Array{Int32,1} = []
  #posmin = Vec3(0,0,0)
  posdist::Vec3{Int32} = Vec3{Int32}(1,1,1)
  Cube_surface_edges(posmin, posdist, xout,yout,zout)
  rect = Rect3D{Int32}(posmin, posdist)
  if (starting == true)
    mesh!(rect, color = (:red, 0.1), transparency = false) #true
  else
    mesh!(rect, color = (:blue, 0.1), transparency = false) #true
  end
  lines!(xout, yout, zout, color = :black)
end
function main(printcacheline::Bool)
  GLMakie.activate!()
  AbstractPlotting.inline!(false)
  lrs::Array{UInt64} = [0,1,2,4,8,12,16,20,24,28,32]#,36,40,44,48]
  cacheline_size::Int32 = 8 # simplify 64 Byte cache line
  #sizehint!(s, n) #???
  for i in lrs
    scene = Scene()
    queue = Queue{LapCoord}()
    seed = Coord(50,50,50)
    seedvec = Vec3{Int32}(seed.x,seed.y,seed.z)
    seedPlot = transform_into_plotcoord(seedvec);
    plotVoxel(seedPlot, true)
    sizes = Coord(100,100,100)
    data = ones(UInt8, 10^6) # 1-dim arary of size 10^6
    lap = 0
    enqueue!(queue, LapCoord(seed,lap))
    data[data_offset(sizes,seed)] = 255
    step::UInt64 = 0
    step += 1
    lastround::UInt64 = 0
    #lrs::Array{UInt64} = [4,5,6,7,8,9,10]
    #lrs::Array{UInt64} = [0,10,20,30,40]
    lastround = i
    srg(step,sizes,data,queue,lastround)
    #qcopy = Queue{LapCoord}()
    #next = iterate(queue) #reverse_iter
    #while next !== nothing
    #  (el,state) = next
    #  enqueue!(qcopy,el)
    #  next = iterate(queue,state)
    #end
    while (isempty(queue) == false)
      elem = dequeue!(queue)
      pos::Coord = elem.coord
      if printcacheline == false
        posvec = Vec3{Int32}(pos.x,pos.y,pos.z)
        posPlot = transform_into_plotcoord(posvec); #BEFOREACTIVE
        plotVoxel(posPlot, false) #BEFOREACTIVE
      else
        pos_do = data_offset(sizes,pos) - 1 #we need real memory offset for div and dont access the data
        offset::Int32 = mod(pos_do,cacheline_size)
        start_cacheline::Int32 = pos_do - offset
        for j in 0:cacheline_size-1
          pos_cache::Int32 = start_cacheline + j
          posvec = data_pos(sizes, pos_cache) #only for plotting!
          posPlot = transform_into_plotcoord(posvec);
          if pos_cache != pos_do
            plotVoxel(posPlot, false)
          else
            plotVoxel(posPlot, true)
          end
        end
      end
      #posvec = Vec3{Int32}(pos.x,pos.y,pos.z)
      #posPlot = transform_into_plotcoord(posvec); BEFOREACTIVE
      #plotVoxel(posPlot, false) BEFOREACTIVE
    end
    if printcacheline == false
      file = string("srgstep_t",string(lastround), ".png")
    else
      file = string("srgcache_step",string(lastround), ".png")
    end
    save(file, scene)
  end
end
printcacheline = true
main(printcacheline)

##TODO 
# TESTING display(scene)   
#  computation 64 * length(queue) + length(queue) * 3 * 32B
#  length(queue) * 160B
