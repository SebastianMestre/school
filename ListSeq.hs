module ListSeq(Seq) where

import Seq
import Par

instance Seq [] where
  emptyS       = []
  singletonS x = [x]
  lengthS      = length
  nthS         = (!!)
  tabulateS    = ListSeq.tabulate
  mapS         = ListSeq.map
  filterS      = ListSeq.filter
  appendS      = (++)
  takeS        = flip take
  dropS        = flip drop
  showtS       = ListSeq.showt
  showlS       = ListSeq.showl
  joinS        = concat
  reduceS      = ListSeq.reduce
  scanS        = ListSeq.scan
  fromList     = id

tabulate :: (Int -> a) -> Int -> [a]
tabulate f n = ListSeq.map f [0..(n-1)]

map :: (a -> b) -> [a] -> [b]
map f [] = []
map f (x:xs) = y : ys
  where (y, ys) = f x ||| ListSeq.map f xs

filter :: (a -> Bool) -> [a] -> [a]
filter p [] = []
filter p (x:xs) | b         = x : ys
                | otherwise = ys
  where (b, ys) = p x ||| ListSeq.filter p xs

showt :: [a] -> TreeView a [a]
showt []  = EMPTY
showt [x] = ELT x
showt xs  = NODE lhs rhs
  where (lhs, rhs) = splitAt (n `div` 2) xs
        n          = length xs

showl :: [a] -> ListView a [a]
showl []     = NIL
showl (x:xs) = CONS x xs

reduce :: (a -> a -> a) -> a -> [a] -> a
reduce f b [] = b
reduce f b s  = f b $ go s
  where go [x] = x
        go xs  = go $ contract f xs

scan :: (a -> a -> a) -> a -> [a] -> ([a], a)
scan f b []  = ([], b)
scan f b [x] = ([b], f b x)
scan f b s   = (expand f s s', p)
  where (s', p) = scan f b $ contract f s

contract :: (a -> a -> a) -> [a] -> [a]
contract f []        = []
contract f [x]       = [x]
contract f (x:x':xs) = y : ys
  where (y, ys) = f x x' ||| contract f xs

expand :: (a -> a -> a) -> [a] -> [a] -> [a]
expand f [] _            = []
expand f [_] (y:_)       = [y]
expand f (x:_:xs) (y:ys) = y : z : zs
  where (z, zs) = f y x ||| expand f xs ys
