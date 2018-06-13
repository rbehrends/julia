# This file is a part of Julia. License is MIT: https://julialang.org/license

# tests the output of the embedding example is correct
using Test

if Sys.iswindows()
    # libjulia needs to be in the same directory as the embedding executable or in path
    ENV["PATH"] = string(Sys.BINDIR, ";", ENV["PATH"])
end

function atleast(s, rx, n)
    m = match(rx, s)
    if m === nothing
	return false
    else
        num = m[1]
	return parse(UInt, num) >= n
    end
end

@test length(ARGS) == 1
@testset "gcext example" begin
    out = Pipe()
    err = Pipe()
    p = run(pipeline(Cmd(ARGS), stdin=devnull, stdout=out, stderr=err), wait=false)
    close(out.in)
    close(err.in)
    out_task = @async readlines(out)
    err_task = @async readlines(err)
    # @test success(p)
    errlines = fetch(err_task)
    lines = fetch(out_task)
    @test length(errlines) == 0
    @test length(lines) == 4
    @test atleast(lines[2], r"([0-9]+) full collections", 10)
    @test atleast(lines[3], r"([0-9]+) partial collections", 1)
    @test atleast(lines[4], r"([0-9]+) finalizer calls", 1)
end
