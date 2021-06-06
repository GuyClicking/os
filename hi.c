/* pro programming */
asm("jmp _main");

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;

struct point {
	uint16 x;
	uint16 y;
};

struct vbe_info_structure {
	char signature[4];// = "VESA";	// must be "VESA" to indicate valid VBE support
	uint16 version;			// VBE version; high byte is major version, low byte is minor version
	uint32 oem;			// segment:offset pointer to OEM
	uint32 capabilities;		// bitfield that describes card capabilities
	uint32 video_modes;		// segment:offset pointer to list of supported video modes
	uint16 video_memory;		// amount of video memory in 64KB blocks
	uint16 software_rev;		// software revision
	uint32 vendor;			// segment:offset to card vendor string
	uint32 product_name;		// segment:offset to card model name
	uint32 product_rev;		// segment:offset pointer to product revision
	char reserved[222];		// reserved for future expansion
	char oem_data[256];		// OEM BIOSes store their strings in this area
} __attribute__ ((packed));

struct vbe_mode_info_structure {
	uint16 attributes;		// deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
	uint8 window_a;			// deprecated
	uint8 window_b;			// deprecated
	uint16 granularity;		// deprecated; used while calculating bank numbers
	uint16 window_size;
	uint16 segment_a;
	uint16 segment_b;
	uint32 win_func_ptr;		// deprecated; used to switch banks from protected mode without returning to real mode
	uint16 pitch;			// number of bytes per horizontal line
	uint16 width;			// width in pixels
	uint16 height;			// height in pixels
	uint8 w_char;			// unused...
	uint8 y_char;			// ...
	uint8 planes;
	uint8 bpp;			// bits per pixel in this mode
	uint8 banks;			// deprecated; total number of banks in this mode
	uint8 memory_model;
	uint8 bank_size;		// deprecated; size of a bank, almost always 64 KB but may be 16 KB...
	uint8 image_pages;
	uint8 reserved0;
 
	uint8 red_mask;
	uint8 red_position;
	uint8 green_mask;
	uint8 green_position;
	uint8 blue_mask;
	uint8 blue_position;
	uint8 reserved_mask;
	uint8 reserved_position;
	uint8 direct_color_attributes;
 
	uint32 framebuffer;		// physical address of the linear frame buffer; write here to draw to the screen
	uint32 off_screen_mem_off;
	uint16 off_screen_mem_size;	// size of memory in the framebuffer but not being displayed on the screen
	uint8 reserved1[206];
} __attribute__ ((packed));

struct IDTDescr {
   uint16 offset_1; // offset bits 0..15
   uint16 selector; // a code segment selector in GDT or LDT
   uint8 zero;      // unused, set to 0
   uint8 type_attr; // type and attributes, see below
   uint16 offset_2; // offset bits 16..31
};

void sleep(int time) {
	asm("mov al, 0x36");
	asm("out 0x43, al");

	int count = 1193182/time;
	//int count = 65536;
	asm("mov al, %0"
		:
		:"g"((uint8)(count&0xFF)));
	asm("out 0x40, al");

	asm("mov al, %0"
		:
		:"g"((uint8)((count&0xFF00)>>8)));
	asm("out 0x40, al");


	int cur = 1;
	int x;
	while (cur != 0) {
		asm("mov al, 0xc0");
		asm("out 0x43, al");

		asm("in %0, 0x40"
			:"=a"(x));
		cur = x;
		asm("in %0, 0x40"
			:"=a"(x));
		cur |= (x<<8);
	}

}

void draw_rect(uint32 fb, uint16 pitch, struct point p, struct point p2, uint8 c) {
	uint8 *pixel = (uint8*)(fb + pitch*p.y + p.x);
	for (int y = 0; y < p2.y-p.y; y++) {
		for (int x = 0; x < p2.x-p.x; x++) {
			*pixel = c;
			pixel++;
		}
		pixel -= (p2.x-p.x);
		pixel += pitch;
	}
}

void clear_screen(uint32 fb, uint16 pitch) {
	struct point a = {0,0}, b = {800,600};
	draw_rect(fb,pitch,a,b,0x00);
}

void draw(uint32 fb, uint16 pitch, struct point p, int c) {
	uint8 *pixel = (uint8*)(fb + pitch*p.y + p.x);
	*pixel = c;
}

#include "font.h"

void print(uint32 fb, uint16 pitch, int c) {
//	const unsigned char ch[] = {
//		0x00, 0x00, 0x7e, 0xff,
//		0xdb, 0xff, 0xff, 0xc3, 0xe7, 0xff, 0xff, 0x7e, 0x00, 0x00, 0x00, 0x00
//	};
	//const uint8 *ch = font + c*16;
	for (int cy = 0; cy < 16; cy++) {
		for (int cx = 0; cx < 8; cx++) {
			draw(fb, pitch, (struct point){cx + 16*(c%50),cy + 32*(c/50)}, font[16*c + cy]&(1<<cx)?0x01:0x08);
			//draw(fb, pitch, (struct point){cx + 16*(c%50),cy + 32*(c/50)}, ch[cy]&(1<<cx)?0x01:0x08);
		}
	}
}

void _main() {
	/*
	// Save bios font
	uint8 font[4096];
	//asm("mov eax, %0"
	//	:
	//	:"g"(&font));
	//asm("mov di, ax");
	asm("mov ax, 0x1130");
	asm("mov bh, 0x06");
	asm("int 0x10");

	int font_es;
	int font_bp;
	asm("mov ax, es");
	asm("mov %0, eax"
		:"=a"(font_es));

	asm("mov ax, bp");
	asm("mov %0, eax"
		:"=a"(font_bp));
	uint8 *font_ptr = font_bp;

	for (int i = 0; i < 4096; i++) {
		font[i] = font_ptr[i];
	}
	*/

    asm("mov eax, 0x4f02");
    asm("mov ebx, 0x4103");
    asm("int 0x10");

	struct vbe_info_structure vi;
	vi.signature[0] = 'V';
	vi.signature[1] = 'E';
	vi.signature[2] = 'S';
	vi.signature[3] = 'A';
	asm("xor eax, eax");
	asm("mov es, eax");
	asm("mov eax, %0"
		:
		:"g"(&vi));
	asm("mov di, ax");

	asm("mov eax, 0x4f00");
	asm("int 0x10");

	struct vbe_mode_info_structure v;
	asm("xor eax, eax");
	asm("mov es, eax");
	asm("mov eax, %0"
		:
		:"g"(&v));
	asm("mov di, ax");

	asm("mov eax, 0x4f01");
	asm("mov ecx, 0x4103");
	asm("int 0x10");

	uint32 gdt;
	asm("lgdt [%0]"
		:
		:"g"(&gdt));

	asm("mov eax, cr0");
	asm("or al, 1");
	asm("mov cr0, eax");

	struct IDTDescr i;
	int *idt_ptr;
	asm("lidt [%0]"
		:
		:"g"(&idt_ptr));

	//uint8 font[4096] =
	//#include "VGA8.F16"
		//;
	/*
	for (int i = 0; i < 256; i++)
		print(v.framebuffer, v.pitch, i);
	*/

	while(1);

	while(1) {
		struct point a={0,0}, b={100,600};
		for (int x = 0; x < 700; x++) {
			//clear_screen(v.framebuffer, v.pitch);
			a.x++;
			b.x++;
			draw_rect(v.framebuffer, v.pitch, a,b,a.x);
			//sleep(1000);
		}
	}
}
