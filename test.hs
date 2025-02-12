pythagoreanTriplets :: Int -> [(Int, Int, Int)]
pythagoreanTriplets n =
  [ (a, b, c)
  | a <- [1 .. n]
  , b <- [a .. n]  -- b starts at a to avoid duplicates (order doesn't matter)
  , c <- [b .. n]  -- similarly, c starts at b
  , a^2 + b^2 == c^2
  ]