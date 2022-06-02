module ListSeq where

import Seq

instance Seq [] where
  emptyS       = []
  singletonS a = [a]
