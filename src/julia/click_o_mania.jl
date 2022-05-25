module click_o_mania
using DataStructures
export Point2D
export findPosition,partition,collectGroups,removeGroup,solve,updateColumn,removeColumn,makeGrid,printGrid

mutable struct Point2D{T}
    x::T
    y::T
end

function findPosition(c::Char, grid::Vector{String})
    y = 1
    for line in grid
        x = findfirst(c,line)
        if x ≠ nothing
            return x,y
        end
        y = y + 1
    end
    0,0
end

const EmptySpace = ' '

mutable struct Workspace
    groups_grid::Array{Int8,2}
    points::Ring{Tuple{Int8,Int8}}
end

function partition(grid::Array{Char,2})
    nrow,ncol = size(grid)
    groups = zeros(Int8,nrow,ncol)
    wrk = Workspace(groups, Ring{Tuple{Int8,Int8}}(nrow*ncol))
    #points = Vector{Tuple{Int8,Int8}}()
    #sizehint!(points,nrow*ncol)
    partition!(grid,wrk)
    groups
end

function partition!(grid::Array{Char,2},wrk::Workspace)
    nrow,ncol = size(grid)
    nextGroup = 1
    fill!(wrk.groups_grid,0)
    for irow in 1:nrow, icol in 1:ncol
        group = wrk.groups_grid[irow,icol]
        if group == 0 && grid[irow,icol] != EmptySpace
            group = nextGroup
            nextGroup = nextGroup + 1
            fillGroupFromPosition!(grid,irow,icol,group,wrk)
        end
    end
end

const Directions = [(-1,0),(0,-1),(0,1),(1,0)]
function fillGroupFromPosition!(grid,irow,icol,group,wrk)
    color = grid[irow,icol]
    nr,nc = size(grid)
    push!(wrk.points,(irow,icol))
    while length(wrk.points) > 0
        r,c = popfirst!(wrk.points)
        wrk.groups_grid[r,c] = group
        for (dr,dc) in Directions
            r1 = r + dr
            (r1 < 1 || r1 > nr) && continue
            c1 = c + dc
            (c1 < 1 || c1 > nc) && continue
            if grid[r1,c1] == color && wrk.groups_grid[r1,c1] == 0
                push!(wrk.points,(r1,c1))
            end
        end
    end
end

stopCondition_slow(groups) = length(groups) == 0 || (length.(groups) .== 1) |> all

function stopCondition(groups)
    for gr in groups
        length(gr) > 1 && return false
    end
    true
end

function solveInternal(grid,level,groups::Union{Vector,Nothing},wrk::Workspace)
    if groups == nothing
        partition!(grid,wrk)
        groups = collectGroups(wrk.groups_grid)
    end
    if stopCondition(groups)
        return length(groups), 0
    else
        min_gi,min_pts = 0,255
        for (gi,g) in enumerate(groups)
            if length(g) > 1
                ngrid = removeGroup(grid,g)
                pts,_ = solveInternal(ngrid,level+1,nothing,wrk)
                if pts < min_pts
                    min_pts = pts
                    min_gi = gi
                end
            end
        end
        #findmin(Pts) #points, group index to remove
        min_pts,min_gi
    end
end

function solve(grid)
    nrow,ncol = size(grid)
    wrk = Workspace(zeros(Int8,nrow,ncol), Ring{Tuple{Int8,Int8}}(nrow*ncol))
    partition!(grid,wrk)
    groups = collectGroups(wrk.groups_grid)
    _,idx = solveInternal(grid,1,groups,wrk)
    idx != 0 ? groups[idx][1] : nothing
end

function collectGroups(groups)
    dgr = Dict( v => [] for v in unique(groups) )
    delete!(dgr,0)
    nrow,ncol = size(groups)
    for irow in 1:nrow, icol in 1:ncol
        group = groups[irow,icol]
        group > 0 && push!( dgr[group], (irow,icol) )
    end
    values(dgr) |> collect
end

function removeGroup(grid,group)
    ncol = size(grid,2)
    ngrid = copy(grid)
    for (r,c) in group
        ngrid[r,c] = EmptySpace
    end
    emptyCols = Int8[]
    for icol in 1:ncol
        updateColumn!(ngrid,icol) && push!(emptyCols,icol)
    end
    for icol in reverse(emptyCols)
        removeColumn!(ngrid,icol)
    end
    ngrid
end

function updateColumn!(grid,icol)
    nrow = size(grid,1)
    for irow in 2:nrow
        if grid[irow,icol] == EmptySpace
            grid[2:irow,icol] = grid[1:irow-1,icol]
            grid[1,icol] = EmptySpace
        end
    end
    (grid[:,icol] .== EmptySpace) |> all
end

function updateColumn(grid,icol)
    ngrid = copy(grid)
    updateColumn!(ngrid,icol)
    ngrid
end

function removeColumn!(grid,icol)
    icol == size(grid,2) && return
    grid[:,icol:end-1] = grid[:,icol+1:end]
    grid[:,end] .= EmptySpace
end

function removeColumn(grid,icol)
    ngrid = copy(grid)
    removeColumn!(ngrid,icol)
    ngrid
end

function makeGrid(rows::Vector{String})
    grid = Array{Char,2}(undef,length(rows),length(rows[1]))
    for (irow,row) in enumerate(rows)
        for (icol,c) in enumerate(row)
            grid[irow,icol]=c
        end
    end
    grid
end

function printGrid(grid)
    grid |> eachrow .|> join .|> println
end

end