push!(LOAD_PATH,"/home/barabasz/src/hackerrank/src/julia")
using Test,Profile
using click_o_mania

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

function profile_partition()
    @profile (for i in 1:10000 partition(grid); end)
end

solve(grid[1:8,1:8])
@profile solve(grid[1:8,1:8])
Profile.print()

