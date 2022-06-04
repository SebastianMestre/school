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

  filterS p s   = joinS $ mapS (\a -> if p a then singletonS a else emptyS) s

  appendS s t   = tabulateS elem (n + m)
    where
      elem i = if i < n then nthS s i else nthS t (i - n)
      n      = lengthS s
      m      = lengthS t

  takeS s n     = Arr.subArray 0 n s

  dropS s n     = Arr.subArray n (lengthS s - n) s

  showtS s
    | n == 0    = EMPTY
    | n == 1    = ELT (firstS s)
    | otherwise = NODE (takeS s n') (dropS s n')
    where
      n  = lengthS s
      n' = n `div` 2

  showlS s
    | n == 0    = NIL
    | otherwise = CONS (firstS s) (dropS s 1)
    where
      n  = lengthS s

  joinS         = Arr.flatten

  reduceS f b s | lengthS s == 0 = b
                | otherwise      = f b $ go s
    where
      go s | lengthS s == 1 = firstS s
           | otherwise      = go $ contractS f s

  scanS f b s   = go s
    where
      go s = case lengthS s of
        0 -> (emptyS, b)
        1 -> (singletonS b, f b (lastS s))
        n -> (tabulate elem n, p)
        where
          (s', p)            = go $ contractS f s
          elem i | even i    = nthS s' (i `div` 2)
                 | otherwise = nthS s' (i `div` 2) `f` nthS s (i-1)

  fromList      = Arr.fromList



contractS f s | even n    = c
              | otherwise = snocS c (lastS s)
  where n      = lengthS s
        c      = tabulateS elem (n `div` 2)
        elem i = nthS s (i*2) `f` nthS s (i*2+1)
firstS s  = nthS s 0
lastS s   = nthS s (lengthS s - 1)
snocS s x = appendS s (singletonS x)

