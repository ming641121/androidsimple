@echo off
if (%1)==() goto error

set WINTYPE=%1
set MAKEFILE=Makefile.%WINTYPE%

cd examples
  echo examples
  if %1==os9nt goto log
  if %1==vxworks_nt goto log
  if %1==ose_nt goto log
  if %1==wce goto log
  cd chro
    echo chro
    nmake -f %MAKEFILE% > make.%WINTYPE%
    type k_chro.fin >%WINTYPE%.obj\k_chro.c
    nmake -f %MAKEFILE% > make2.%WINTYPE%
  cd ..
  :log
  cd log
    echo log
    if %1==os9nt goto oslog
    if %1==vxworks_nt goto vxlog
    nmake -f %MAKEFILE% > make.%WINTYPE%
    type k_logo.fin >%WINTYPE%.obj\k_logo.c
    nmake -f %MAKEFILE% > make2.%WINTYPE%
    goto endlog
    :oslog
    bmake -f %MAKEFILE% > make.%WINTYPE%
    type k_logo.fin >%WINTYPE%.obj\k_logo.c
    bmake -f %MAKEFILE% > make2.%WINTYPE%
    goto endlog
    :vxlog
    make -f %MAKEFILE% > make.%WINTYPE%
    type k_logo.fin >%WINTYPE%.obj\k_logo.c
    make -f %MAKEFILE% > make2.%WINTYPE%
    :endlog
  cd ..
  cd mirror
    echo mirror
    if %1==os9nt goto osmirr
    if %1==vxworks_nt goto vxmirr
    nmake -f %MAKEFILE% > make.%WINTYPE%
    type k_mirr.fin >%WINTYPE%.obj\k_mirr.c
    nmake -f %MAKEFILE% > make2.%WINTYPE%
    goto endmirr
    :osmirr
    bmake -f %MAKEFILE% > make.%WINTYPE%
    type k_mirr.fin >%WINTYPE%.obj\k_mirr.c
    bmake -f %MAKEFILE% > make2.%WINTYPE%
    goto endmirr
    :vxmirr
    make -f %MAKEFILE% > make.%WINTYPE%
    type k_mirr.fin >%WINTYPE%.obj\k_mirr.c
    make -f %MAKEFILE% > make2.%WINTYPE%
    :endmirr
  cd ..
  cd note
    echo note
    if %1==os9nt goto osnote
    if %1==vxworks_nt goto vxnote
    nmake -f %MAKEFILE% > make.%WINTYPE%
    type k_note.fin >%WINTYPE%.obj\k_note.c
    nmake -f %MAKEFILE% > make2.%WINTYPE%
    goto endnote
    :osnote
    bmake -f %MAKEFILE% > make.%WINTYPE%
    type k_note.fin >%WINTYPE%.obj\k_note.c
    bmake -f %MAKEFILE% > make2.%WINTYPE%
    goto endnote
    :vxnote
    make -f %MAKEFILE% > make.%WINTYPE%
    type k_note.fin >%WINTYPE%.obj\k_note.c
    make -f %MAKEFILE% > make2.%WINTYPE%
    :endnote
  cd ..
  cd thrm
    echo thrm
    if %1==os9nt goto osthrm
    if %1==vxworks_nt goto vxthrm
    nmake -f %MAKEFILE% > make.%WINTYPE%
    type k_thrm.fin >%WINTYPE%.obj\k_thrm.c
    nmake -f %MAKEFILE% > make2.%WINTYPE%
    goto endthrm
    :osthrm
    bmake -f %MAKEFILE% > make.%WINTYPE%
    type k_thrm.fin >%WINTYPE%.obj\k_thrm.c
    bmake -f %MAKEFILE% > make2.%WINTYPE%
    goto endthrm
    :vxthrm
    make -f %MAKEFILE% > make.%WINTYPE%
    type k_thrm.fin >%WINTYPE%.obj\k_thrm.c
    make -f %MAKEFILE% > make2.%WINTYPE%
    :endthrm
  cd ..
  cd thst
    echo thst
    if %1==os9nt goto osthst
    if %1==vxworks_nt goto vxthst
    nmake -f %MAKEFILE% > make.%WINTYPE%
    type k_thst.fin >%WINTYPE%.obj\k_thst.c
    nmake -f %MAKEFILE% > make2.%WINTYPE%
    goto endthst
    :osthst
    bmake -f %MAKEFILE% > make.%WINTYPE%
    type k_thst.fin >%WINTYPE%.obj\k_thst.c
    bmake -f %MAKEFILE% > make2.%WINTYPE%
    goto endthst
    :vxthst
    make -f %MAKEFILE% > make.%WINTYPE%
    type k_thst.fin >%WINTYPE%.obj\k_thst.c
    make -f %MAKEFILE% > make2.%WINTYPE%
    :endthst
  cd ..
  cd tmon
    echo tmon
    if %1==os9nt goto ostmon
    if %1==vxworks_nt goto vxtmon
    nmake -f %MAKEFILE% > make.%WINTYPE%
    type k_tmon.fin >%WINTYPE%.obj\k_tmon.c
    nmake -f %MAKEFILE% > make2.%WINTYPE%
    goto endtmon
    :ostmon
    bmake -f %MAKEFILE% > make.%WINTYPE%
    type k_tmon.fin >%WINTYPE%.obj\k_tmon.c
    bmake -f %MAKEFILE% > make2.%WINTYPE%
    goto endtmon
    :vxtmon
    make -f %MAKEFILE% > make.%WINTYPE%
    type k_tmon.fin >%WINTYPE%.obj\k_tmon.c
    make -f %MAKEFILE% > make2.%WINTYPE%
    :endtmon
  cd ..
  cd type
    echo type
    if %1==os9nt goto ostype
    if %1==vxworks_nt goto vxtype
    nmake -f %MAKEFILE% > make.%WINTYPE%
    type k_type.fin >%WINTYPE%.obj\k_type.c
    nmake -f %MAKEFILE% > make2.%WINTYPE%
    goto endtype
    :ostype
    bmake -f %MAKEFILE% > make.%WINTYPE%
    type k_type.fin >%WINTYPE%.obj\k_type.c
    bmake -f %MAKEFILE% > make2.%WINTYPE%
    goto endtype
    :vxtype
    make -f %MAKEFILE% > make.%WINTYPE%
    type k_type.fin >%WINTYPE%.obj\k_type.c
    make -f %MAKEFILE% > make2.%WINTYPE%
    :endtype
  cd ..
  cd vcr
    echo vcr
    cd auto
      echo auto
      if %1==os9nt goto osauto
      if %1==vxworks_nt goto vxauto
      nmake -f %MAKEFILE% > make.%WINTYPE%
      type k_vcr.fin >%WINTYPE%.obj\k_vcr.c
      nmake -f %MAKEFILE% > make2.%WINTYPE%
      goto endauto
      :osauto
      bmake -f %MAKEFILE% > make.%WINTYPE%
      type k_vcr.fin >%WINTYPE%.obj\k_vcr.c
      bmake -f %MAKEFILE% > make2.%WINTYPE%
      goto endauto
      :vxauto
      make -f %MAKEFILE% > make.%WINTYPE%
      type k_vcr.fin >%WINTYPE%.obj\k_vcr.c
      make -f %MAKEFILE% > make2.%WINTYPE%
      :endauto
    cd ..
    cd byhand
      echo byhand
      if %1==os9nt goto osbyhand
      if %1==vxworks_nt goto vxbyhand
      nmake -f %MAKEFILE% > make.%WINTYPE%
      type k_vcr.fin >%WINTYPE%.obj\k_vcr.c
      nmake -f %MAKEFILE% > make2.%WINTYPE%
      goto endbyhand
      :osbyhand
      bmake -f %MAKEFILE% > make.%WINTYPE%
      type k_vcr.fin >%WINTYPE%.obj\k_vcr.c
      bmake -f %MAKEFILE% > make2.%WINTYPE%
      goto endbyhand
      :vxbyhand
      make -f %MAKEFILE% > make.%WINTYPE%
      type k_vcr.fin >%WINTYPE%.obj\k_vcr.c
      make -f %MAKEFILE% > make2.%WINTYPE%
      :endbyhand
    cd ..
  cd ..
cd ..
cd combo
  echo combo
  cd thrmthst
    echo thrmthst
    if %1==os9nt goto osauto
    if %1==vxworks_nt goto vxauto
    nmake -f %MAKEFILE% startover
    goto endauto
    :osauto
    bmake -f %MAKEFILE% startover
    goto endauto
    :vxauto
    make -f %MAKEFILE% startover
    :endauto
    if exist make.%1 del make.%1
    if exist make2.%1 del make2.%1
  cd ..
cd ..
goto done

:error
  @echo off
  echo "usage: test [w32|os9nt|vxworks_nt|ose_nt]"

:done

