code segment
main proc far
assume cs:code
buffer db 512 dup(0)
file   db 'a:\hdboot.dat',0
handle dw  ?
error1 db  'open error'
error2 db  'read error'
error3 db  'int 13 error'
waring db  '  It will use a:\hdboot.dat to restore your hard disk mastboot area!!! It may cause  some proplem'
       db  '  even lost all data on hard disk,and can boot from hard disk!!!!'
       db  '  make sure your have a:\hdboot.dat,and make sure it is good and useable'
       db  '  It is a idea to use a:\save.exe to have a hard disk boot backup in another  disk  now.'
       db  '  If there are somethinh wrong after this restore operation,you could use this restore again by '
       db  'useing the a:\hdboot.dat you just backup in another disk ( insert the disk in drive a ) '
       db  'press [Y/y] to restore now!!!press any other key to quit now!..............'
ok     db  'succefully !!! OK!! '
start:
     push ds
     sub ax,ax
     push ax
     mov ax,seg code
     mov ds,ax
     mov es,ax
     mov ax,0002h
     int 10h
     mov bp,offset waring
     mov dx,0902h
     mov bx,005ah
     mov ax,1301h
     mov cx,580
     int 10h
     mov ah,0
     int 16h
     cmp ah,15h
     jne exit
     mov dx,offset file
     mov ax,03d00h
     int 21h
     jc  err1
     mov handle ,ax
     lea dx,buffer
     mov bx,handle
     mov cx,512
     mov ah,3fh
     int 21h
     cmp ax,512
     jc err2
     mov bx,offset buffer
     mov ax,seg code
     mov es,ax
     mov ax,0301h
     mov cx,0001h
     mov dx,0080h
     int 13h
     cmp ah,0
     jne err3
     mov bx,handle
     mov ah,3eh
     int 21h
     jmp ok1
err1:mov bp,offset error1
     mov cx,17
     jmp print
err2:mov bp,offset error2
     mov cx,17
     jmp print
err3:mov bp,offset error3
     mov cx,17
     jmp print
ok1: mov bp,offset ok
     mov cx,17
     jmp print
print:mov ax,seg code
      mov es,ax
      mov dx,0609h
      mov bx,00a8h
      mov ax ,1301h
      int 10h
exit:
      ret
main endp
code ends
end start





