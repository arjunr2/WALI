print("Hello from WALI lua")

local sum = 0
for i = 1, 10 do sum = sum + i end
print("sum(10):", sum)

local function fact(n)
    if n == 0 then return 1 end
    return n * fact(n-1)
end

local t0 = os.clock()
local f = fact(5)
local elapsed = os.clock() - t0
print("fact(5):", f)
print(string.format("elapsed: %.4fs", elapsed))
