module ListSeq where

import Seq
import Par

instance Seq [] where
  emptyS       = []

  singletonS a = [a]

  lengthS = length

  nthS = (!!)

  tabulateS f n = mapS f [0..(n-1)]

  mapS f s = go s
    where
      go [] = []
      go (x:xs) = let
        (y, ys) = f x ||| go xs
        in y:ys

  filterS p s = go s
    where
      go [] = []
      go (x:xs) = let
        (b, ys) = p x ||| go ys
          in if b then x:ys else ys

  appendS = (++)

  takeS [] _                 = []
  takeS (x:xs) n | n <= 0    = []
                 | otherwise = head s : take (tail s) (n-1)

  dropS = flip drop

  showtS []  = EMPTY
  showtS [x] = ELT x
  showtS xs  = NODE lhs rhs
    where (lhs, rhs) = splitAt (n `div` 2) xs
          n = length xs

  showlS [] = NIL
  showlS (x:xs) = CONS x xs

  joinS      = undefined

  reduceS    = undefined

  scanS      = undefined

  fromList = id
