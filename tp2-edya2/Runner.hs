
import Expr
import Seq
import ListSeq
import ArrSeq

symbols n = take n $ map (VarE.("x"++).show) [0..]

run :: ((a -> a -> a) -> a -> s a -> b) -> (a -> a -> a) -> a -> s a -> s a -> b
run f c e es _ = f c e es

-- simple :: Seq s => ((Expr -> Expr -> Expr) -> Expr -> s Expr -> b) -> Int -> s Expr -> b
simple f n aux = run f Mul (VarE "b") (fromList $ symbols n) aux

mtyA :: Arr a
mtyA = emptyS

mtyL :: Arr a
mtyL = emptyS
