module Expr where

import Seq

data Expr = VarE String
          | Mul Expr Expr

instance Show Expr where
  show (VarE s)  = s
  show (Mul l r) = inParen l ++ " ⊕ " ++ inParen r
    where inParen (VarE s) = s
          inParen e        = "(" ++ show e ++ ")"

