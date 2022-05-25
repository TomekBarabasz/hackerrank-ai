module DataStructures

export Ring

mutable struct Ring{T}
    first::Number
    nelem::Number
    capacity::Number
    table::Vector{T}
    function Ring{T}(capacity::Int) where {T}
        ring = new{T}(1,0,capacity,Vector{T}(undef,capacity))
        ring
    end
end

import Base.isempty, Base.push!, Base.popfirst!, Base.length;

function isempty(r::Ring{T}) where {T}
    r.nelem == 0
end

function push!(r::Ring{T},elem::T)::Bool where {T}
    r.nelem == r.capacity && return false
    next = r.first + r.nelem
    idx = ifelse(next <= r.capacity,next,1)
    @inbounds r.table[idx] = elem
    r.nelem = r.nelem + 1
    true
end

function push!(r::Ring{T},elem)::Bool where {T}
    push!(r,convert(T,elem))
end

function popfirst!(r::Ring{T}) where {T}
    @boundscheck r.nelem > 0 || return nothing;
    idx = r.first
    r.nelem = r.nelem - 1
    next = r.first + 1
    r.first = ifelse(next <= r.capacity,next,1)
    @inbounds r.table[idx]
end

function length(r::Ring{T}) where {T}
    r.nelem
end

end