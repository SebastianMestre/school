module ArrSeq where

import Arr
import Seq

instance Seq Arr where
  emptyS        = Arr.empty

  singletonS a  = fromList [a]

  lengthS       = Arr.length

  nthS          = (Arr.!)

  tabulateS     = Arr.tabulate

  mapS f s      = tabulateS (\i -> f (nthS s i)) (lengthS s)

  filterS p s   = joinS $ mapS (\a -> if p a then singletonS a else empty)

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
    | otherwise = CONS (nthS s 0) (dropS 1 s)

  joinS         = Arr.flatten

  reduceS       = undefined

  scanS f b s   = tabulateS elem 
    where s' = scanS f b $ contract f s
    elem i = case i of
      0                  -> nthS s 0
      _ | i `mod` 2 == 0 -> f (nthS s' ((i `div` 2)-1)) (nthS s i)
        | otherwise      -> nthS s' (i `div` 2)

  contract f s = tabulateS elem (lengthS s `div` 2)
    where elem i = 

  fromList      = Arr.fromList



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
