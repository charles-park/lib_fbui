# lib_fbui
JIG에 사용하는 Framebuffer control library (UI화면 설정).

```
root@server:~/lib_fbui# ./lib_fbui -h

Usage: ./lib_fbui [-DrgbxywhfntscCi]
  -D --device    device to use (default /dev/fb0)
  -r --red       pixel red hex value.(default = 0)
  -g --green     pixel green hex value.(default = 0)
  -b --blue      pixel blue hex value.(default = 0)
  -x --x_pos     framebuffer memory x position.(default = 0)
  -y --y_pos     framebuffer memory y position.(default = 0)
  -w --width     reference width for drawing.
  -h --height    reference height for drawing.
  -f --fill      drawing fill box.(default empty box)
  -n --thickness drawing line thickness.(default = 1)
  -t --text      drawing text string.(default str = "text"
  -s --scale     scale of text.
  -c --color     background rgb(hex) color.(ARGB)
  -C --clear     clear framebuffer(r = g = b = 0)
  -i --info      framebuffer info display.
  -F --font      Hangul font select
                 0 MYEONGJO
                 1 HANBOOT
                 2 HANGODIC
                 3 HANPIL
                 4 HANSOFT

  Useage : ./lib_fbui -I fbui.cfg -s 3 -F 2
```
