module ListSeq where

import Seq
import Par

instance Seq [] where
  emptyS        = []
  singletonS a  = [a]
  lengthS       = length
  nthS          = (!!)
  tabulateS f n = mapS f [0..(n-1)]
  mapS f s      = go s
    where
      go [] = []
      go (x:xs) = let
        (y, ys) = f x ||| go xs
        in y:ys
  filterS p s   = go s
    where
      go [] = []
      go (x:xs) = let
        (b, ys) = p x ||| go ys
          in if b then x:ys else ys
  appendS       = (++)
  -- takeS [] _    = []
  -- takeS (x:xs) n
  --   | n <= 0    = []
  --   | otherwise = x : takeS xs (n-1)
  takeS         = flip take -- Vale esto ???
  dropS         = flip drop -- Vale esto ???
  showtS []     = EMPTY
  showtS [x]    = ELT x
  showtS xs     = NODE lhs rhs
    where
      (lhs, rhs) = splitAt (n `div` 2) xs
      n          = length xs
  showlS []     = NIL
  showlS (x:xs) = CONS x xs
  joinS         = undefined

  reduceS f b [] = b
  reduceS f b s  = f b $ go s
    where
      go [x] = x
      go xs  = go $ contractS f s

  scanS f b s   = go s
    where
      go []  = ([], b)
      go [x] = ([b], f b x)
      go s   = (expandList s s', p)
        where
          (s', p)            = go $ contractS f s

  fromList      = id

contractList f []        = []
contractList f [x]       = [x]
contractList f (x:x':xs) = y:ys
  where (y, ys) = f x x' ||| contractList f xs

expandList f [] _            = []
expandList f [_] (y:_)       = [y]
expandList f (x:_:xs) (y:ys) = y:z:zs
  where (z, zs) = f y x ||| expandList f xs ys
