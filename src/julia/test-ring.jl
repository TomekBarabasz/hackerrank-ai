push!(LOAD_PATH,"/home/barabasz/src/hackerrank/src/julia")
using Test
using DataStructures

@testset "test isempty" begin
    ring = Ring{Int8}(3)
    @test isempty(ring) == true
    @test length(ring) == 0
    push!(ring,1)
    @test isempty(ring) == false
    @test length(ring) == 1
end;

@testset "test push!" begin
    ring = Ring{Int8}(3)
    @test push!(ring,1)==true
    @test length(ring)==1
    @test push!(ring,2)==true
    @test length(ring)==2
    @test push!(ring,3)==true
    @test length(ring)==3
    @test push!(ring,4)==false
    @test length(ring)==3
end;

@testset "test popfirst" begin
    ring = Ring{Int8}(3)
    push!(ring,1)
    push!(ring,2)
    push!(ring,3)
    @test isempty(ring) == false
    @test length(ring)==3
    @test popfirst!(ring) == 1
    @test length(ring)==2
    @test popfirst!(ring) == 2
    @test length(ring)==1
    @test popfirst!(ring) == 3
    @test length(ring)==0
    @test isempty(ring) == true
    @test popfirst!(ring) == nothing;
    @test length(ring)==0
    @test isempty(ring) == true
end;
