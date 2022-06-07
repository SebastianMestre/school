module ArrSeq(Arr, Seq) where

import Arr
import Seq

instance Seq Arr where
  emptyS     = ArrSeq.empty
  singletonS = ArrSeq.singleton
  lengthS    = ArrSeq.length
  nthS       = ArrSeq.nth
  tabulateS  = ArrSeq.tabulate
  mapS       = ArrSeq.map
  filterS    = ArrSeq.filter
  appendS    = ArrSeq.append
  takeS      = ArrSeq.take
  dropS      = ArrSeq.take
  showtS     = ArrSeq.showt
  showlS     = ArrSeq.showl
  joinS      = ArrSeq.join
  reduceS    = ArrSeq.reduce
  scanS      = ArrSeq.scan
  fromList   = ArrSeq.fromList

empty :: Arr a
empty = Arr.empty

singleton :: a -> Arr a
singleton a = Arr.fromList [a]

length :: Arr a -> Int
length = Arr.length

nth :: Arr a -> Int -> a
nth = (Arr.!)

tabulate :: (Int -> a) -> Int -> Arr a
tabulate = Arr.tabulate

fromList :: [a] -> Arr a
fromList = Arr.fromList

map :: (a -> b) -> Arr a -> Arr b
map f s    = ArrSeq.tabulate elem (lengthS s)
  where elem i = f $ nthS s i

filter :: (a -> Bool) -> Arr a -> Arr a
filter p s = Arr.flatten $ ArrSeq.map f s
  where f a | p a       = ArrSeq.singleton a
            | otherwise = ArrSeq.empty

append :: Arr a -> Arr a -> Arr a
append s t = ArrSeq.tabulate elem (n + m)
  where
    elem i = if i < n then nthS s i else nthS t (i - n)
    n      = lengthS s
    m      = lengthS t

take :: Arr a -> Int -> Arr a
take s n   = Arr.subArray 0 n s

drop :: Arr a -> Int -> Arr a
drop s n   = Arr.subArray n (lengthS s - n) s

showt :: Arr a -> TreeView a (Arr a)
showt s
  | n == 0    = EMPTY
  | n == 1    = ELT (first s)
  | otherwise = NODE (takeS s n') (dropS s n')
  where
    n  = lengthS s
    n' = n `div` 2

showl :: Arr a -> ListView a (Arr a)
showl s
  | n == 0    = NIL
  | otherwise = CONS (first s) (dropS s 1)
  where
    n  = lengthS s

join :: Arr (Arr a) -> Arr a
join = Arr.flatten

reduce :: (a -> a -> a) -> a -> Arr a -> a
reduce f b s | lengthS s == 0 = b
             | otherwise      = f b $ go s
  where
    go s | lengthS s == 1 = first s
         | otherwise      = go $ contract f s

scan :: (a -> a -> a) -> a -> Arr a -> (Arr a, a)
scan f b s   = go s
  where
    go s = case lengthS s of
      0 -> (emptyS, b)
      1 -> (singletonS b, f b (ArrSeq.last s))
      n -> (ArrSeq.tabulate elem n, p)
      where
        (s', p)            = go $ contract f s
        elem i | even i    = nthS s' (i `div` 2)
               | otherwise = nthS s' (i `div` 2) `f` nthS s (i-1)

contract :: (a -> a -> a) -> Arr a -> Arr a
contract f s | even n    = c
                | otherwise = snoc c (ArrSeq.last s)
  where n      = lengthS s
        c      = ArrSeq.tabulate elem (n `div` 2)
        elem i = nthS s (i*2) `f` nthS s (i*2+1)

first :: Arr a -> a
first s  = nthS s 0

last :: Arr a -> a
last s   = nthS s (lengthS s - 1)

snoc :: Arr a -> a -> Arr a
snoc s x = append s (singletonS x)

