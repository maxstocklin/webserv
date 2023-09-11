#!/usr/bin/php


<?php echo "
HTTP/1.1 200 OK
Content-Type: text/html
Connection: close 

<!doctype html>
<html>
  <head>
    <title>This is the title sof the site!</title>
  </head>
  <body>
    <p style=\"background-color: gray;\">Thisss is an example paragraph.</p>
    <p>Anything in the <strong>body</strong> tag will appear on the page, just like this <strong>p</strong> tag and its contents.</p>
    <div   style=\"width: 100px; height: 100px; background-color: blueviolet;\">
      <p></p>
    </div>
    <div   style=\"width: 100px; height: 100px; background-color: green;\">
      <p></p>
      <img src=\"https://tinypic.host/images/2023/09/05/4214c1c2a3552a032972ff4ecb46144a.md.jpeg\" alt=\"\">
    </div>
  
  </body>
</html>

"; ?>