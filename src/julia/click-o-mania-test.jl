push!(LOAD_PATH,"/home/barabasz/src/hackerrank/src/julia")
using Test,Profile
using click_o_mania

@testset "test findPosition" begin
    @test findPosition('5',["123","456","789"]) == (2,2)
    @test findPosition('2',["123","456","789"]) == (2,1)
    @test findPosition('7',["123","456","789"]) == (1,3)
end

@testset "test partition" begin
    grid = makeGrid(["112","132","122"])
    @test partition(grid) == [1 1 2;1 3 2;1 2 2]

    grid = makeGrid(["11122",
                    "31344",
                    "33354",
                    "67855",
                    "68795"])
    @test partition(grid) == [1 1 1 2 2;3 1 3 4 4;3 3 3 5 4;6 7 8 5 5;6 9 10 11 5]
end;

@testset "test updateColumn" begin
    grid = makeGrid(["123",
                     "345",
                     "687"])
    @test updateColumn(grid,1) == grid

    grid = makeGrid([" 23",
                    "313",
                    "687"])
    @test updateColumn(grid,1) == grid

    grid = makeGrid(["123",
                    " 13",
                    "687"])
    @test updateColumn(grid,1) == makeGrid([" 23","113","687"])

    grid = makeGrid(["123",
                    "313",
                    " 87"])
    @test updateColumn(grid,1) == makeGrid([" 23","113","387"])
end

@testset "test removeColumn" begin
    grid = makeGrid(["12 ",
                    "45 ",
                    "78 "])
    @test removeColumn(grid,3) == grid

    grid = makeGrid(["1 3",
                    "4 6",
                    "7 9"])
    @test removeColumn(grid,2) == makeGrid(["13 ","46 ","79 "])

    grid = makeGrid([" 23",
                    " 56",
                    " 89"])
    @test removeColumn(grid,1) == makeGrid(["23 ","56 ","89 "])
end
@testset "test remove group" begin
    grid = makeGrid(["brbb",
                    "rryy",
                    "yrby",
                    "brry",
                    "yrrb"])
    groups = collectGroups(partition(grid))
    grid = removeGroup(grid,groups[5])
end

@testset "test solve" begin
    grid = makeGrid(["y  ",
                    "y  ",
                    "r  "])
    @test solve(grid) == (1,1)

    grid = makeGrid(["b  ",
                    "y  ",
                    "bbb",
                    "ybb"])
    @test solve(grid) == (3,1)

    grid = makeGrid(["  b",
                    "b y",
                    "yby",
                    "byy",
                    "ybb"])
    @test solve(grid) == (5,2)

    grid = makeGrid(["brbb",
                    "rryy",
                    "yrby",
                    "brry",
                    "yrrb"])
    groups = collectGroups(partition(grid))
    sln = solve(grid)
    @test sln ∈ groups[5]
end

grid = makeGrid(["BRBBBBRRBB",
"RBRBBRRRRR",
"BRBBRRRBBB",
"BRRRBBBRBR",
"BBBBRBRRBR",
"BRBRRBRRRB",
"BBRBRBRBBB",
"RBRBRBRRBR",
"BRRRRRBBRB",
"RRRRBRRRRB",
"BRRRBBBRBR",
"RRBRRBBBRB",
"RBBBBBBBBB",
"BRBBRBRBBR",
"RBRBBBRRRB",
"RBRBRBRBBB",
"BRRRBRBRRR",
"RRBRBBBBBR",
"RBBRBBRBRB",
"BBBBRRBBRB"])

println("timimg partition")
NR,NC=size(grid)
for nrow in 5:NR
    @time partition(grid[1:nrow,:])
end

println("timing solve")
for nrow in 1:7
    @time solve(grid[1:nrow,:])
end

#println("profiling partition")
#@profile (for i in 1:10000 partition(grid); end)
#Profile.print()

#println("solving 9x9 grid")
#@time solve(grid[1:9,1:9])
#@profile solve(grid[1:9,1:9])
#Profile.print()

#for s in 5:9
#    @time solve(grid[1:s,1:s])
#end
