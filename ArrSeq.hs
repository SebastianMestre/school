module ArrSeq(Arr, Seq) where

import qualified Arr
import Seq

type Arr a = Arr.Arr a

instance Seq Arr.Arr where
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
map f s = tabulate elem (Arr.length s)
  where elem i = f $ nth s i

filter :: (a -> Bool) -> Arr a -> Arr a
filter p s = Arr.flatten $ ArrSeq.map f s
  where f a | p a       = singleton a
            | otherwise = empty

append :: Arr a -> Arr a -> Arr a
append s t = tabulate elem (n + m)
  where
    elem i = if i < n then nth s i else nth t (i - n)
    n      = Arr.length s
    m      = Arr.length t

take :: Arr a -> Int -> Arr a
take s n = Arr.subArray 0 n s

drop :: Arr a -> Int -> Arr a
drop s n = Arr.subArray n (Arr.length s - n) s

showt :: Arr a -> TreeView a (Arr a)
showt s
  | n == 0    = EMPTY
  | n == 1    = ELT (first s)
  | otherwise = NODE (ArrSeq.take s n') (ArrSeq.drop s n')
  where
    n  = Arr.length s
    n' = n `div` 2

showl :: Arr a -> ListView a (Arr a)
showl s
  | n == 0    = NIL
  | otherwise = CONS (first s) (ArrSeq.drop s 1)
  where
    n  = Arr.length s

join :: Arr (Arr a) -> Arr a
join = Arr.flatten

reduce :: (a -> a -> a) -> a -> Arr a -> a
reduce f b s | Arr.length s == 0 = b
             | otherwise         = f b $ go s
  where
    go s | Arr.length s == 1 = first s
         | otherwise         = go $ contract f s

scan :: (a -> a -> a) -> a -> Arr a -> (Arr a, a)
scan f b s
  | n == 0    = (empty, b)
  | n == 1    = (singleton b, f b (ArrSeq.last s))
  | otherwise = (tabulate elem n, p)
  where
    n                  = Arr.length s
    (s', p)            = scan f b $ contract f s
    elem i | even i    = nth s' (i `div` 2)
           | otherwise = nth s' (i `div` 2) `f` nth s (i-1)

contract :: (a -> a -> a) -> Arr a -> Arr a
contract f s | even n    = c
             | otherwise = snoc c (ArrSeq.last s)
  where n      = Arr.length s
        c      = tabulate elem (n `div` 2)
        elem i = nth s (i*2) `f` nth s (i*2+1)

first :: Arr a -> a
first s = nth s 0

last :: Arr a -> a
last s = nth s (Arr.length s - 1)

snoc :: Arr a -> a -> Arr a
snoc s x = append s (singleton x)

