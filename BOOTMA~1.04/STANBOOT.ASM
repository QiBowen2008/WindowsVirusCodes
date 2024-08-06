;安装标准的启动主引导程序
PartLoad equ 600h
BootLoc  equ 7c00h
TableBegin equ 01beh+600h
code segment 
main proc far
assume cs:code
	  org 600h
coldboot:
	 cli
	 xor ax,ax
	 mov ss,ax
	 mov sp,7c00h
	 mov si,sp
	 push ax
	 pop es
	 push ax
	 pop ds
	 sti
	   
	 cld   
	 mov di,PartLoad
	 mov cx,100h
	 repne movsw
	 db 0eah
	 dw offset Continue,0000h
Continue:
	 mov si,TableBegin
	 mov bl,4
FindBoot:
	 cmp byte ptr[si],80h
	 je SaveRec
	 cmp byte ptr[si],0
	 jne Invalid
	 add si,10h
	 dec bl
	 jnz FindBoot
	 int 18h
SaveRec:
	 mov dx,[si]   
	 mov cx,[si+2]
	 mov bp,si
FindNext:
	 add si,10h               
	 dec bl
	 jz SetRead
	 cmp byte ptr[si],0
	 je FindNext
Invalid: 
	 mov si,offset ErrMsg1
PrintStr:
	 lodsb
	 cmp al,0
	 je DeadLock
	 push si
	 mov bx,7
	 mov ah,0eh
	 int 10h

	 pop si
	 jmp short PrintStr
DeadLock:
	 jmp short DeadLock
SetRead:
	 mov di,5
ReadBoot:
	 mov bx,BootLoc
	 mov ax,201h
	 push di
	 int 13h

	 pop di
	 jnc GoBoot
	 xor ax,ax
	 int 13h
	 dec di
	 jnz ReadBoot
	 mov si,offset ErrMsg2
	 jmp short PrintStr

GoBoot:
	 mov si,offset ErrMsg3
	 mov di,7c00h+1feh
	 cmp word ptr[di],0aa55h
	 jne PrintStr
	 mov si,bp
	 db 0eah,00h,7ch,00h,00h
ErrMsg1  db 'Invalid partition table',0
ErrMsg2  db 'Error loading operating system',0
ErrMsg3  db 'Missing operating system',0
Tail:    db 'NULL'
buffer1  db 512 dup(0)
buffer2  db 512 dup(0)
start:   
	push ds
	xor ax,ax
	push ax
	mov ax,seg code
	mov es,ax
	mov ds,ax
	mov ax,0002h
	int 10h
	mov bp,offset waring
	mov dx,0902h
	mov bx,005ah
	mov ax,1301h
	mov cx,200
	int 10h
	mov ah,0
	int 16h
	cmp ah,15h
	jne  exit
	mov bx,offset buffer1
	mov ax,0201h
	mov cx,0001h
	mov dx,0080h
	int 13h
	cmp ah,0
	jne errorpri
	mov di,offset buffer2
	add di,1beh
	mov si,offset buffer1
	add si,1beh
	mov cx,40h
	repne movsb
	mov di,offset buffer2
	mov si,offset coldboot
	mov cx,(offset Tail)-(offset coldboot)
	repne movsb
	mov  buffer2+1feh,055h
	mov  buffer2+1ffh,0aah
	mov ax,0301h
	mov bx,offset buffer2
	mov cx,0001h
	mov dx,0080h
	int 13h
	cmp ah,0
	jne errorpri
	mov bp,seg code
	mov es,bp
	mov bp,offset ok
	mov dx,0506h
	mov bx,00a8h
	mov ax,1301h
	mov cx,20
	int 10h
	jmp exit
errorpri:
	mov bp,offset errorint
	mov dx,0909h
	mov bx,00dah
	mov ax,1301h
	mov cx,10h
	int 10h
exit:   ret
errorint db 'int 13 error'        
ok       db 'sucessfully!!!OK!!...............'
waring   db 'This will make  your hard disk have passwordcheck function.That is say you can select to'
	 db ' restore it.press [Y/y] to multiboot your system.!!!.....press any other key to quite now'
	 db '..............................................'

main endp
code ends
end start    


    

