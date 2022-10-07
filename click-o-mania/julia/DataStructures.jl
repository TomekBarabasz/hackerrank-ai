module DataStructures
    include("ring.jl")
    include("block_list.jl")

    export Ring
    export BlockList,BlockDictionary,PositionsVector,fillBlockList!
end
