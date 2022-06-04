module ArrSeq where

import Arr
import Seq

instance Seq Arr where
  emptyS        = Arr.empty

  singletonS a  = Arr.fromList [a]

  lengthS       = Arr.length

  nthS          = (Arr.!)

  tabulateS     = Arr.tabulate

  mapS f s      = tabulateS (\i -> f (nthS s i)) (lengthS s)

  filterS p s   = joinS $ mapS (\a -> if p a then singletonS a else empty) s

  appendS s t   = tabulateS elem (sLen + tLen)
    where
      elem i = if i < sLen then nthS s i else nthS t (i - sLen)
      sLen   = lengthS s
      tLen   = lengthS t

  takeS s n     = Arr.subArray 0 n s

  dropS s n     = Arr.subArray n (lengthS s - n) s

  showtS s
    | lenS == 0 = EMPTY
    | lenS == 1 = ELT (nthS s 0)
    | otherwise = NODE (takeS s lenS') (dropS s lenS')
    where
      lenS  = lengthS s
      lenS' = lenS `div` 2

  showlS s
    | lenS == 0 = NIL
    | otherwise = CONS (nthS s 0) (dropS s 1)
    where
      lenS  = lengthS s

  joinS         = Arr.flatten

  reduceS       = undefined

  scanS f b s   = let
    s' = scan s
    n' = lengthS s'
    suffix = nthS s' (n' - 1)
    prefix = subArray 0 (n' - 1) s'
    in (prefix, suffix)
    where

      scan s = case lengthS s of
        0 -> singletonS b
        1 -> snoc (singletonS b) (f b (lastS s))
        n -> snoc (tabulate elem n) (lastS s')
        where
          c  = contract s
          s' = scan c
          elem i | even i    = nthS s' (i `div` 2)
                 | otherwise = nthS s' (i `div` 2) `f` nthS s (i-1)

      contract s | even n    = c
                 | otherwise = snoc c (lastS s)
        where n = lengthS s
              c = tabulateS elem (n `div` 2)
              elem i = nthS s (i*2) `f` nthS s (i*2+1)


  fromList      = Arr.fromList

snoc s x = appendS s (singletonS x)
lastS s = nthS s (lengthS s - 1)


{--

-- implementacion naive

data Tree a = Leaf a | Node (Tree a) (Tree a)

reduceS f b s = case lengthS s of
  0 -> b
  n -> f b (reduceT f $ toTree s)
    where
      toTree s = case lengthS s of
        1 -> Leaf (nthS s 0)
        n -> Node (toTree (takeS s pp))
                  (toTree (dropS s pp))
          where pp = 2 ** ilg (n-1)
      
      reduceT f (Leaf x)   = x
      reduceT f (Node l r) = f (reduceT l) (reduceT r)

-- fusionando y manipulando un poco se llega esto

reduceS f b s = case lengthS s of
  0 -> b
  n -> f b $ go s
  where
    go s = case lengthS s of
      1 -> nthS s 0
      n -> f (go $ takeS s pp)
             (go $ dropS s pp)
        where pp = 2 ** ilg (n-1)

-- despues podemos aprovechar que el lado izquierdo siempre recursiona con una potencia de dos

reduceS f b s = case lengthS s of
  0 -> b
  n -> f b $ go s
  where
    go s = case lengthS s of
      1 -> nthS s 0
      n -> f (go' (pp `div` 2) $ takeS s pp)
             (go               $ dropS s pp)
        where pp = 2 ** ilg (n-1)
    go' pp s = case lengthS s of
      1 -> nthS s 0
      n -> f (go' (pp `div` 2) $ takeS s pp)
             (go' (pp `div` 2) $ dropS s pp)

--}


data Expr = VarE String
          | Mul Expr Expr

instance Show Expr where
  show (VarE s)  = s
  show (Mul l r) = inParen l ++ " ⊕ " ++ inParen r
    where inParen (VarE s) = s
          inParen e        = "(" ++ show e ++ ")"

runvars e es = scanS Mul (VarE e) (Arr.fromList $ map VarE es)

example = runvars "b" ["x0", "x1", "x2", "x3", "x4", "x5"]
