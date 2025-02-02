/* Definitions of target machine for GNU compiler,
   for IBM RS/6000 POWER running AIX version 4.3.
   Copyright (C) 1998-2016 Free Software Foundation, Inc.
   Contributed by David Edelsohn (edelsohn@gnu.org).

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published
   by the Free Software Foundation; either version 3, or (at your
   option) any later version.

   GCC is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING3.  If not see
   <http://www.gnu.org/licenses/>.  */

/* The macro SUBTARGET_OVERRIDE_OPTIONS is provided for subtargets, to
   get control in TARGET_OPTION_OVERRIDE.  */

#define SUBTARGET_OVERRIDE_OPTIONS					\
do {									\
  if (TARGET_64BIT && ! TARGET_POWERPC64)				\
    {									\
      rs6000_isa_flags |= OPTION_MASK_POWERPC64;			\
      warning (0, "-maix64 requires PowerPC64 architecture remain enabled"); \
    }									\
  if (TARGET_SOFT_FLOAT && TARGET_LONG_DOUBLE_128)			\
    {									\
      rs6000_long_double_type_size = 64;				\
      if (global_options_set.x_rs6000_long_double_type_size)		\
	warning (0, "soft-float and long-double-128 are incompatible");	\
    }									\
  if (TARGET_POWERPC64 && ! TARGET_64BIT)				\
    {									\
      error ("-maix64 required: 64-bit computation with 32-bit addressing not yet supported"); \
    }									\
} while (0);

#undef ASM_SPEC
#define ASM_SPEC "-u %{maix64:-a64 %{!mcpu*:-mppc64}} %(asm_cpu)"

/* Common ASM definitions used by ASM_SPEC amongst the various targets
   for handling -mcpu=xxx switches.  */
#undef ASM_CPU_SPEC
#define ASM_CPU_SPEC \
"%{!mcpu*: %{!maix64: \
  %{!mpowerpc64: %(asm_default)} \
  %{mpowerpc64: -mppc64}}} \
%{mcpu=power3: -m620} \
%{mcpu=power4: -m620} \
%{mcpu=powerpc: -mppc} \
%{mcpu=rs64a: -mppc} \
%{mcpu=601: -m601} \
%{mcpu=602: -mppc} \
%{mcpu=603: -m603} \
%{mcpu=603e: -m603} \
%{mcpu=604: -m604} \
%{mcpu=604e: -m604} \
%{mcpu=620: -m620} \
%{mcpu=630: -m620}"

#undef	ASM_DEFAULT_SPEC
#define ASM_DEFAULT_SPEC "-mppc"

#undef TARGET_OS_CPP_BUILTINS
#define TARGET_OS_CPP_BUILTINS()     \
  do                                 \
    {                                \
      builtin_define ("_AIX43");     \
      TARGET_OS_AIX_CPP_BUILTINS (); \
    }                                \
  while (0)

#undef CPP_SPEC
#define CPP_SPEC "%{posix: -D_POSIX_SOURCE}\
   %{ansi: -D_ANSI_C_SOURCE}\
   %{maix64: -D__64BIT__}\
   %{mpe: -I%R/usr/lpp/ppe.poe/include}\
   %{pthread: -D_THREAD_SAFE}"

/* The GNU C++ standard library requires that these macros be 
   defined.  */
#undef CPLUSPLUS_CPP_SPEC			
#define CPLUSPLUS_CPP_SPEC			\
  "-D_ALL_SOURCE				\
   %{maix64: -D__64BIT__}			\
   %{mpe: -I%R/usr/lpp/ppe.poe/include}		\
   %{pthread: -D_THREAD_SAFE}"

#undef TARGET_DEFAULT
#define TARGET_DEFAULT 0

#undef PROCESSOR_DEFAULT
#define PROCESSOR_DEFAULT PROCESSOR_PPC604e

/* AIX does not support Altivec.  */
#undef  TARGET_ALTIVEC
#define TARGET_ALTIVEC 0
#undef  TARGET_ALTIVEC_ABI
#define TARGET_ALTIVEC_ABI 0
#undef  TARGET_EXTRA_BUILTINS
#define TARGET_EXTRA_BUILTINS 0


/* Define this macro as a C expression for the initializer of an
   array of string to tell the driver program which options are
   defaults for this target and thus do not need to be handled
   specially when using `MULTILIB_OPTIONS'.

   Do not define this macro if `MULTILIB_OPTIONS' is not defined in
   the target makefile fragment or if none of the options listed in
   `MULTILIB_OPTIONS' are set by default.  *Note Target Fragment::.  */

#undef	MULTILIB_DEFAULTS
#define	MULTILIB_DEFAULTS { "mcpu=common" }

#undef LIB_SPEC
#define LIB_SPEC "%{pg:-L%R/lib/profiled -L%R/usr/lib/profiled}\
   %{p:-L%R/lib/profiled -L%R/usr/lib/profiled}\
   %{!maix64:%{!shared:%{g*:-lg}}}\
   %{mpe:-L%R/usr/lpp/ppe.poe/lib -lmpi -lvtd}\
   %{pthread:-L%R/usr/lib/threads -lpthreads -lc_r %R/usr/lib/libc.a}\
   %{!pthread:-lc}"

#undef LINK_SPEC
#define LINK_SPEC "-bpT:0x10000000 -bpD:0x20000000 %{!r:-btextro} -bnodelcsect\
   %{static:-bnso %(link_syscalls) } %{shared:-bM:SRE %{!e:-bnoentry}}\
   %{!maix64:%{!shared:%{g*: %(link_libg) }}} %{maix64:-b64}\
   %{mpe:-binitfini:poe_remote_main}"

#undef STARTFILE_SPEC
#define STARTFILE_SPEC "%{!shared:\
   %{maix64:%{pg:gcrt0_64%O%s}%{!pg:%{p:mcrt0_64%O%s}%{!p:crt0_64%O%s}}}\
   %{!maix64:\
     %{pthread:%{pg:gcrt0_r%O%s}%{!pg:%{p:mcrt0_r%O%s}%{!p:crt0_r%O%s}}}\
     %{!pthread:%{pg:gcrt0%O%s}%{!pg:%{p:mcrt0%O%s}%{!p:crt0%O%s}}}}}"

/* AIX 4.3 typedefs ptrdiff_t as "long" while earlier releases used "int".  */

#undef PTRDIFF_TYPE
#define PTRDIFF_TYPE "long int"

/* AIX 4.2 and above provides initialization and finalization function
   support from linker command line.  */
#undef HAS_INIT_SECTION
#define HAS_INIT_SECTION

#undef LD_INIT_SWITCH
#define LD_INIT_SWITCH "-binitfini"

/* The IBM AIX 4.x assembler doesn't support forward references in
   .set directives.  We handle this by deferring the output of .set
   directives to the end of the compilation unit.  */
#define TARGET_DEFERRED_OUTPUT_DEFS(DECL,TARGET) true

/* This target uses the aix64.opt file.  */
#define TARGET_USES_AIX64_OPT 1

#define TARGET_AIX_VERSION 43

#undef TARGET_LIBC_HAS_FUNCTION
#define TARGET_LIBC_HAS_FUNCTION no_c99_libc_has_function
