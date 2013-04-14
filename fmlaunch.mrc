on ^*:HOTLINK:*.*.*.*:*:{
  if (($gettok($1, 1, 46) isnum 0-255) && ($gettok($1, 2, 46) isnum 0-255) && ($gettok($1, 3, 46) isnum 0-255)) {
    if ($count($1, $chr(58)) == 1) {
      if ($gettok($1, 2, 58) isnum 0-65535) {
        return
        } else {
        halt
      }
    }
    return
  }

  halt
}

on *:HOTLINK:*.*.*.*:*: {
  ;XXX wonder how to avoid hardcoding path?
  run "C:\Program Files\Fragmon\fragmon.exe" $1
}
