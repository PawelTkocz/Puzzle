import ScanSlicer
from subprocess import call

puzzle_pieces = ScanSlicer.sliceScans()
call(["make"])
print("\n")
call(["./puzzle_solver", str(puzzle_pieces)])