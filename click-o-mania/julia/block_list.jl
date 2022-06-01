mutable struct BlockGroup
    id::UInt16
    first::UInt16
    last::UInt16
end

PositionsVector = Vector{Tuple{Int8,Int8}}
mutable struct BlockList
    positions::PositionsVector
    blocks::Vector{BlockGroup}
end

function BlockList(size::Int)
    BlockList(
        PositionsVector(undef,size),
        Vector{BlockGroup}()
    )
end

BlockDictionary = Dict{Int16,Vector{Tuple{Int8,Int8}}}
function fillBlockList!(blockDict::BlockDictionary,blockList::BlockList)
    empty!(blockList.positions)
    empty!(blockList.blocks)
    first = 1 
    for (id,points) in blockDict
        append!(blockList.positions,points)
        last = length(blockList.positions)
        push!(blockList.blocks, BlockGroup(id,first,last))
        first = last + 1
    end
end
