module Expand where

import Shape
import Formula
import Crosscutting

expand :: Shape -> Form ()
expand PointS            = lengthF pos
expand (TranslatedS x s) = withPos (pos - vec x) s
expand (InflatedS x s)   = expand s - lit x
expand (ExtrudedS x s)   = withPos (extrude x pos) s
expand (RotatedXyS a s)  = withPos (mat (makeRotationMatrix a) * pos) s
expand (RepeatedXS x s)  = withPos (periodic x pos) s
expand (RepeatedXyS k s) = expand s -- TODO
expand (UnionS s1 s2)    = minF (expand s1) (expand s2)


withPos :: Form () -> Shape -> Form ()
withPos f s = letF VectorF "pos" f $ expand s

extrude :: Float3 -> Form () -> Form ()
extrude x f = f - clampF f (vec $ negate3 x) (vec x)

periodic :: Float -> Form () -> Form ()
periodic x f = updX (\x -> modF (x + offset) period - offset) f
  where period = lit x
        offset = lit (x / 2)

varF = VarF ()
appF = AppF ()
letF = LetF ()
prjF = PrjF ()

pos = varF "pos"

getX = prjF XF
getY = prjF YF
getZ = prjF ZF

lengthF f = appF LengthF [f]
minF f1 f2 = appF MinF [f2, f2]
modF x y = appF ModF [x, y]
clampF f1 f2 f3 = appF ClampF [f1, f2, f3]

updX f e =
  letF VectorF "__old" e $
  appF MkVecF [f $ getX old, getY old, getZ old]
  where old = varF "__old"

lit x = LitF x
vec (x, y, z) = appF MkVecF [lit x, lit y, lit z]
mat (vx, vy, vz) = appF MkMatF [vec vx, vec vy, vec vz]

makeRotationMatrix :: Float -> (Float3, Float3, Float3)
makeRotationMatrix angle =
  ((cosAngle, -sinAngle, 0),
   (sinAngle,  cosAngle, 0),
   (0,         0,        1))
  where
  cosAngle = cos (-angle)
  sinAngle = sin (-angle)

negate3 (x, y, z) = (-x, -y, -z)
