code segment
main proc far
assume cs:code
buffer db 512 dup(0)
handle dw ?
file   db 'a:\hdboot.dat',0
error1 db 'crert error,maybe a:\hdboot.dat already exist'
error2 db 'open error       '
error3 db 'write error      '
waring db 'make sure your hard disk  can boot,press [Y/y] key to create your boot backup in a:\hdboot.dat  now'
       db ' .. and the old file will be lost....when your hard disk have boot problem,you can use a:restore.exe to'
       db ' restore your old hard boot from a:\hdboot.dat that you will backup now...press any key to quit now!!!!'   
ok     db 'sucessfully,ok!! '
start:
     push ds
     sub ax,ax
     push ax
     mov ax,0002
     int 10h
     mov ax,seg code
     mov ds,ax    
     mov es,ax
     mov bp,offset waring
     mov dx,0902h
     mov bx,005ah
     mov cx,307
     mov ax,1301h
     int 10h
     mov ah,0
     int 16h
     cmp ah,15h
     jne exit     
     mov ah,3ch  
     mov cx,0
     mov dx,offset file
     int 21h
     jc err1
     mov ah,3dh
     mov al,1
     int 21h
     jc err2
     mov handle,ax
     mov ax,0201h
     mov bx,offset buffer
     mov cx,0001
     mov dx,0080h
     int 13h
     mov dx,offset buffer
     mov bx,handle
     mov cx,512
     mov ah,40h
     int 21h
     cmp ax,512
     jne err3
     mov bp,offset ok
     mov cx,4
     jmp printf
err1:mov bp,offset error1
     mov cx,45
     jmp printf
err2:mov bp,offset error2
     mov cx,17
     jmp printf
err3:mov bp,offset error3
     mov cx,17
     jmp printf
printf:
    mov cx,19
    mov dx,0d0ah
    mov bx,00a8h
    mov ah,13h
    mov al,1
    int 10h
    mov bx,handle
    mov ah,3eh
    int 21h
exit:
    ret
main endp
code ends
end start

