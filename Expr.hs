module Expr where

data Expr = VarE String
          | Mul Expr Expr

instance Show Expr where
  show (VarE s)  = s
  show (Mul l r) = inParen l ++ " ⊕ " ++ inParen r
    where inParen (VarE s) = s
          inParen e        = "(" ++ show e ++ ")"

stringsToExprSeq = fromList . map VarE

testscan f e es = f Mul (VarE e) $ stringsToExprSeq es

example f = testscan f "b" ["x0", "x1", "x2", "x3", "x4", "x5"]
