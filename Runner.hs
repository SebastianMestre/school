
import Expr
import Arr
import Seq
import ListSeq
import ArrSeq

stringsToExprSeq l = Seq.fromList (map VarE l)

testscan f e es = f Mul (VarE e) $ stringsToExprSeq es

example f = testscan f "b" ["x0", "x1", "x2", "x3", "x4", "x5"]

symbols n = take n $ map (("x"++).show) [0..]
