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
function debug_printQueue(queue::Queue{LapCoord})
  println("queue2 check_enqueue_vox:")
  next = iterate(queue) #reverse_iter
  while next !== nothing
    (el,state) = next
    println(el)
    next = iterate(queue,state)
  end
end
function check_enqueue_vox(sizes::Coord, data::Array{UInt8}, queue::Queue{LapCoord}, queue2::Queue{LapCoord}, vpos::Coord, lap::UInt64, step::Base.RefValue{Int64}, ok1::Bool)
  offset::Int32 = data_offset(sizes, vpos)
  if (data[offset] == 1)
    if ok1 == true
      if step[] == 200 #210 ### CHANGES THIS ###
        #println("length: ", length(queue2))
        #debug_printQueue(queue2)
        #isempty(queue2) == false
        if length(queue2) == 0
          #println("added ", LapCoord(vpos,lap), "to queue2")
          enqueue!(queue2, LapCoord(vpos,lap))
          #debug_printQueue(queue2)
        end
        step[] = 0
        data[offset] = 255
        return
      end
      step[] += 1
      data[offset] = 255
    else
      data[offset] = 100 #thread2 marks with 100
    end
    enqueue!(queue, LapCoord(vpos,lap))
  end
end
function processNeighbor(sizes::Coord, n::Int64, data::Array{UInt8}, queue::Queue{LapCoord}, queue2::Queue{LapCoord}, elem::LapCoord, step::Base.RefValue{Int64}, ok1::Bool)
  pos::Coord = elem.coord
  if n == 0
    if (pos.x+1 < sizes.x)
      vpos = Coord(pos.x+1,pos.y,pos.z)
      check_enqueue_vox(sizes, data, queue, queue2, vpos, elem.lap+1,step, ok1)
    end
  elseif n==1
    if (pos.x-1 >= 0)
      vpos = Coord(pos.x-1,pos.y,pos.z)
      check_enqueue_vox(sizes, data, queue, queue2, vpos, elem.lap+1,step, ok1)
    end
  elseif n==2
    if (pos.y+1 < sizes.y)
      vpos = Coord(pos.x,pos.y+1,pos.z)
      check_enqueue_vox(sizes, data, queue, queue2, vpos, elem.lap+1,step, ok1)
    end
  elseif n==3
    if (pos.y-1 >= 0)
      vpos = Coord(pos.x,pos.y-1,pos.z)
      check_enqueue_vox(sizes, data, queue, queue2, vpos, elem.lap+1,step, ok1)
    end
  elseif n==4
    if (pos.z+1 < sizes.z)
      vpos = Coord(pos.x,pos.y,pos.z+1)
      check_enqueue_vox(sizes, data, queue, queue2, vpos, elem.lap+1,step, ok1)
    end
  else # n==5
    if (pos.z-1 >= 0)
      vpos = Coord(pos.x,pos.y,pos.z-1)
      check_enqueue_vox(sizes, data, queue, queue2, vpos, elem.lap+1,step, ok1)
    end
  end
end
function srg(sizes::Coord, data::Array{UInt8}, queue1::Queue{LapCoord}, queue2::Queue{LapCoord}, lastround::UInt64)
  step::Base.RefValue{Int64} = Ref(1)
  ok1::Bool = true
  ok2::Bool = true
  started::Bool = false
  startedtmp::Bool = false
  _coord = Coord(0,0,0)
  elem1 = LapCoord(_coord, 0)
  elem2 = LapCoord(_coord, 0)
  while ( ok1 == true || ok2 == true )
    if (length(queue1) != 0 && first(queue1).lap <= lastround)
      elem1 = dequeue!(queue1)
      ok1 = true
    else
      ok1 = false
    end
    #println("length: ", length(queue2))
    if (length(queue2) != 0 && first(queue2).lap <= lastround)
      elem2 = dequeue!(queue2)
      #println(elem2) # elem2
      ok2 = true
    else
      ok2 = false
    end
    for n in 0:5 #n for 6 neighbors 0..5
      if ok1 == true
        processNeighbor(sizes, n, data, queue1, queue2, elem1, step, true) #called by thread1
      end
      if ok2 == true
        processNeighbor(sizes, n, data, queue2, queue2, elem2, step, false) #called by thread2
      end
    end
    if startedtmp == true
      started = true
      step[] = 0
    end
    #println("lap:",lap)
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
function main()
  GLMakie.activate!()
  AbstractPlotting.inline!(false)
  lrs::Array{UInt64} = [11] #[0,1,2,3,4,5,6,7,8,9,10,11]
                        #,36,40,44,48]
  cacheline_size::Int32 = 8 # simplify 64 Byte cache line
  #sizehint!(s, n) #???
  for i in lrs
    scene = Scene()
    queue = Queue{LapCoord}()
    queue2 = Queue{LapCoord}()
    seed = Coord(50,50,50)
    seedvec = Vec3{Int32}(seed.x,seed.y,seed.z)
    seedPlot = transform_into_plotcoord(seedvec);
    plotVoxel(seedPlot, true)
    sizes = Coord(100,100,100)
    data = ones(UInt8, 10^6) # 1-dim arary of size 10^6
    lap = 0
    pos = Vec3{Int32}(0,0,0)
    enqueue!(queue, LapCoord(seed,lap))
    data[data_offset(sizes,seed)] = 255
    lastround::UInt64 = 0
    lastround = i
    srg(sizes,data,queue,queue2,lastround)
    for j in 1:length(data)
      if data[j] == 255
        pos = data_pos(sizes, convert(Int32,j - 1))
        posPlot = transform_into_plotcoord(pos)
        plotVoxel(posPlot,false) #plot thread0
      end
      if data[j] == 100
        pos = data_pos(sizes, convert(Int32,j - 1))
        posPlot = transform_into_plotcoord(pos)
        plotVoxel(posPlot,true) #plot thread1
      end
    end

    #while (length(queue) != 0)
    #  elem = dequeue!(queue)
    #  pos::Coord = elem.coord
    #  posvec = Vec3{Int32}(pos.x,pos.y,pos.z)
    #  posPlot = transform_into_plotcoord(posvec);
    #  plotVoxel(posPlot, false) #can not plot multiple different colors or surface not visible
    #end
    #println("length: ", length(queue2))
    #while (length(queue2) != 0)
    #  elem = dequeue!(queue2)
    #  pos::Coord = elem.coord
    #  posvec = Vec3{Int32}(pos.x,pos.y,pos.z)
    #  posPlot = transform_into_plotcoord(posvec);
    #  plotVoxel(posPlot, true)
    #end
    file = string("srg2threads",string(lastround), ".png")
    save(file, scene)
  end
end
main()

##TODO 
# TESTING display(scene)   
#  computation 64 * length(queue) + length(queue) * 3 * 32B
#  length(queue) * 160B
