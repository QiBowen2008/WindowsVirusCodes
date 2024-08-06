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
	pop  es
	push ax
	pop  ds
	sti
	cld
	mov di,600h
	mov cx,100h
	repne movsw
	db 0eah
	dw offset continue,0000h
old      dw 1                       
select   dw ?                     
continue:
	mov ax,0002h
	int 10h
	mov ax,0701h
	mov bh,12h
	call scroll
	mov ax,1301h
	mov dx,021ch
	mov bx,012h
	mov cx,15
	mov bp,offset hello
	int 10h  
startmenu:
	  mov ax,old
	  mov select, ax
iniparam:
	  mov di,1
	  mov bp,offset string
	  mov dx,081dh

menu:
	  mov bx,028h
	  cmp di,select
	  jne nextitem
	  mov bx,00dah
nextitem:
	  mov cx,15
	  mov ax,1301h
	  int 10h
	  add dh,2
	  add bp,15
	  inc di
	  cmp di,5
	  jne menu
key:
	  mov ah,0
	  int 16h
	  cmp ah,48h
	  je up
	  cmp ah,50h
	  je down
	  cmp ah,1ch
	  jne key
	  jmp beginboot
up:
	  cmp select,1
	  jne notop  
	  mov select,4
	  jmp iniparam
notop:
	  dec select
	  jmp iniparam
down:
	  cmp select,4
	  jne nobottom
	  mov select,1
	  jmp iniparam
nobottom:
	  inc select
	  jmp iniparam
beginboot:
	mov si,600h+1beh
	mov bx,1
mastboot:
	cmp bx,select    
	jne disable
	mov byte ptr[si],80h
	mov dx,[si]
	mov cx,[si+2]
	mov bp,si
	jmp next
disable:
	mov byte ptr[si],0
next:   add si,10h
	inc bx
	cmp bx,5
	jne mastboot
readini:
	mov di,5             ;try timer
readboot:
	mov bx,7c00h
	mov ax,0201h
	int 13h
	cmp ah,0        
	jmp goboot       
	xor ax,ax    
	int 13h
	dec di
	jnz readboot
	jmp errorboot
goboot:
	mov ax,select
	cmp ax,old
	je bootnow
	mov old,ax
	mov bx,0600h
	mov ax,0301h
	mov cx,0001h
	mov dx,0080h
	int 13h
	cmp ah,0
	jne errorboot
bootnow:
	mov ax,0601h
	mov bh,07h
	call scroll
	mov dx,0905h
	mov bh,0
	mov ah,2
	int 10h
	mov si,bp
	db 0eah
	dw 7c00h,0000h
errorboot:
	mov bp,offset errbootdat
	mov cx,10
	jmp print
print:
	mov dx,0909h
	mov bx,00a8h
	mov ax,1301h
	int 10h
errbootdat db 'boot error'
hello    db '    WELLCOME    '
string   db ' 1. ---------- '
	 db ' 2. ---------- '           ;;only 15 charater
	 db ' 3. ---------  '
	 db ' 4. ---------- '
    
      
scroll proc near
      mov cx,18h
lp:
      push cx   
      mov cx,0110h
      mov dx,143bh
      int 10h
      call delay1
      pop cx    
      loop lp
      ret
scroll endp
delay1 proc near
      mov cx,040h       
wait1:push cx
      mov cx,00efeh
delay:loop delay      
      pop cx
      loop wait1
      ret 
delay1 endp      
end1:
buffer1 db 512 dup(0)
buffer2 db 512 dup(0)
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
	mov cx,500
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
	mov cx,(offset end1)-(offset coldboot)
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
waring   db 'This will make  your hard disk have multi-boot function.That is say you can select to'
	 db ' boot UNIX,OS/2,WINDOWS NT,WINDOWS 95.....but this just suppose you have four prime partion . '
	 db 'and you have to modify the menu item to fit your system. you could have a try to see what it is'
	 db ' this would't destroy your partion table.but if want restore your old mastboot area  '
	 db '  you have to use save.exe to save the old date now.if something wrong you could use restore.exe to '
	 db ' restore it.press [Y/y] to multiboot your system.!!!.....press any other key to quite now'
	 db '..............................................'
main endp
code ends
end start    


    

