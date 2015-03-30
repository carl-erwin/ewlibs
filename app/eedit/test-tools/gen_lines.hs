import System.Environment

genLines :: Int -> Int -> Int -> IO ()
genLines startLineNum lastLine numChar =
    do
    let a  = (take numChar $ repeat 'x')
    putStrLn $ (show startLineNum) ++ " " ++ (show a)
    if startLineNum < lastLine
    then genLines (startLineNum + 1) lastLine numChar
    else return ()

main = do
    progName <- getProgName
    argList  <- getArgs
    let startLineNum = rInt (argList !! 0)
    let lastLine     = rInt (argList !! 1)
    let numChar      = rInt (argList !! 2)
    if length argList == 3
    then genLines startLineNum lastLine numChar
    else putStrLn $ "usage : " ++ (show progName) ++ " startLineNum numLine numChar"

rInt :: String -> Int 
rInt a = read a
