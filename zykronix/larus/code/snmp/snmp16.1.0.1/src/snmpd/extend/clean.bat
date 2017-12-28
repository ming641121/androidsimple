@echo off
if (%1)==() goto error

set MAKEFILE=Makefile.%1

cd examples
  echo examples
  if %1==vxworks_nt goto log
  if %1==os9nt goto log
  if %1==ose goto log
  if %1==ose_nt goto log
  if %1==wce goto log
  cd chro
    echo chro
    nmake -f %MAKEFILE% startover
    if exist make.%1 del make.%1
    if exist make2.%1 del make2.%1
  cd ..
  :log
  cd log 
    echo log
    if %1==os9nt goto oslog
    if %1==vxworks_nt goto vxlog
    nmake -f %MAKEFILE% startover
    goto endlog
    :oslog
    bmake -f %MAKEFILE% startover
    goto endlog
    :vxlog
    make -f %MAKEFILE% startover  
    :endlog
    if exist make.%1 del make.%1
    if exist make2.%1 del make2.%1
  cd ..  
  cd mirror
    echo mirror
    if %1==os9nt goto osmirr
    if %1==vxworks_nt goto vxmirr
    nmake -f %MAKEFILE% startover
    goto endmirr
    :osmirr
    bmake -f %MAKEFILE% startover  
    goto endmirr
    :vxmirr
    make -f %MAKEFILE% startover
    :endmirr
    if exist make.%1 del make.%1
    if exist make2.%1 del make2.%1
  cd ..
  cd note
    echo note
    if %1==os9nt goto osnote
    if %1==vxworks_nt goto vxnote
    nmake -f %MAKEFILE% startover
    goto endnote
    :osnote
    bmake -f %MAKEFILE% startover  
    goto endnote
    :vxnote
    make -f %MAKEFILE% startover
    :endnote
    if exist make.%1 del make.%1
    if exist make2.%1 del make2.%1
  cd ..
  cd thrm
    echo thrm
    if %1==os9nt goto osthrm
    if %1==vxworks_nt goto vxthrm
    nmake -f %MAKEFILE% startover
    goto endthrm
    :osthrm
    bmake -f %MAKEFILE% startover
    goto endthrm
    :vxthrm
    make -f %MAKEFILE% startover
    :endthrm
    if exist make.%1 del make.%1
    if exist make2.%1 del make2.%1
  cd ..
  cd thst
    echo thst
    if %1==os9nt goto osthst
    if %1==vxworks_nt goto vxthst
    nmake -f %MAKEFILE% startover
    goto endthst
    :osthst
    bmake -f %MAKEFILE% startover
    goto endthst
    :vxthst
    make -f %MAKEFILE% startover
    :endthst
    if exist make.%1 del make.%1
    if exist make2.%1 del make2.%1
  cd ..
  cd tmon
    echo tmon
    if %1==os9nt goto ostmon
    if %1==vxworks_nt goto vxtmon
    nmake -f %MAKEFILE% startover
    goto endtmon
    :ostmon
    bmake -f %MAKEFILE% startover
    goto endtmon
    :vxtmon
    make -f %MAKEFILE% startover
    :endtmon
    if exist make.%1 del make.%1
    if exist make2.%1 del make2.%1
  cd ..
  cd type
    echo type
    if %1==os9nt goto ostype
    if %1==vxworks_nt goto vxtype
    nmake -f %MAKEFILE% startover
    goto endtype
    :ostype
    bmake -f %MAKEFILE% startover
    goto endtype
    :vxtype
    make -f %MAKEFILE% startover
    :endtype
    if exist make.%1 del make.%1
    if exist make2.%1 del make2.%1
  cd ..
  cd vcr
    echo vcr
      cd auto
      echo auto
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
    cd byhand
      echo byhand
      if %1==os9nt goto osbyhand
      if %1==vxworks_nt goto vxbyhand
      nmake -f %MAKEFILE% startover
      goto endbyhand
      :osbyhand
      bmake -f %MAKEFILE% startover
      goto endbyhand
      :vxbyhand
      make -f %MAKEFILE% startover
      :endbyhand
      if exist make.%1 del make.%1
      if exist make2.%1 del make2.%1
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
  echo "usage: clean [w32|os9nt|vxworks_nt|ose_nt]"

:done

