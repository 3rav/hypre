#!/bin/ksh 
#BHEADER***********************************************************************
# (c) 2000   The Regents of the University of California
#
# See the file COPYRIGHT_and_DISCLAIMER for a complete copyright
# notice, contact person, and disclaimer.
#
# $Revision$
#EHEADER***********************************************************************

function MpirunString
{ # generate default mpirun or psub command
  typeset -L4 HOST=$(hostname)
  typeset -i POE_NUM_PROCS=1
  typeset -i POE_NUM_NODES=1
  typeset -i CPUS_PER_NODE=1
  HOST=${HOST%%.*}
  case $HOST in
    fros*) CPUS_PER_NODE=16
      POE_NUM_PROCS=$2
      ((POE_NUM_NODES=POE_NUM_PROCS + CPUS_PER_NODE - 1))
      ((POE_NUM_NODES=POE_NUM_NODES / CPUS_PER_NODE))
      shift
      shift
      RunString="poe $* -procs $POE_NUM_PROCS -nodes $POE_NUM_NODES"
      ;;
    blue*) CPUS_PER_NODE=4
      POE_NUM_PROCS=$2
      ((POE_NUM_NODES=POE_NUM_PROCS + CPUS_PER_NODE - 1))
      ((POE_NUM_NODES=POE_NUM_NODES / CPUS_PER_NODE))
      shift
      shift
      RunString="poe $* -procs $POE_NUM_PROCS -nodes $POE_NUM_NODES"
      ;;
    tckk*) shift
      RunString="prun -n$*"
      ;;
    peng*) shift
      RunString="prun -n$*"
      ;;
    perr*|achi*|poin*|esak*|ares*|hypn*|weyl*|juve*) MACHINES_FILE="hostname"
      if [ ! -f $MACHINES_FILE ]
      then
        hostname > $MACHINES_FILE
      fi
      RunString="mpirun -machinefile $MACHINES_FILE $*"
      ;;
    gps*) RunString="mpirun $*"
      ;;
    lx*) RunString="mpirun $*"
      ;;
    tc*) RunString="mpirun $*"
      ;;
    tux*|perr*|achi*) MACHINES_FILE="hostname"
      if [ ! -f $MACHINES_FILE ]
      then
        hostname > $MACHINES_FILE
      fi
      RunString="mpirun -machinefile $MACHINES_FILE $*"
      ;;
    vivi*) RunString="mpirun $*"
      ;;
    ript*) RunString="mpirun $*"
      ;;
    janu*) shift
      RunString="yod -sz $*"
      ;;
    *) RunString="mpirun $*"
      ;;
  esac
}
function CheckBatch
{ # determine if host can process psub (batch queues)
  typeset -i BATCH_MODE=0
  typeset -L4 HOST=$(hostname)
  HOST=${HOST%%.*}
  case $HOST in
    fros*) BATCH_MODE=1
      ;;
    blue*) BATCH_MODE=1
      ;;
    tckk*) BATCH_MODE=1
      ;;
    peng*) BATCH_MODE=1
      ;;
    gps*) BATCH_MODE=1
      ;;
    lx*) BATCH_MODE=0
      ;;
    tc*) BATCH_MODE=1
      ;;
    *) BATCH_MODE=0
      ;;
  esac
  return $BATCH_MODE
}
function CalcNodes
{ # determine the "number of nodes" desired by dividing the
  # "number of processes" by the "number of CPU's per node"
  # which can't be determined dynamically (real ugly hack)
  typeset -i NUM_PROCS=1
  typeset -i NUM_NODES=1
  typeset -i CPUS_PER_NODE
  typeset -L4 HOST=$(hostname)
  HOST=${HOST%%.*}              # remove possible ".llnl.gov"
  case $HOST in
    fros*) CPUS_PER_NODE=16
      ;;
    blue*) CPUS_PER_NODE=4
      ;;
    tckk*) CPUS_PER_NODE=4
      ;;
    peng*|ilx*) CPUS_PER_NODE=2
      ;;
    gps*) CPUS_PER_NODE=4
      ;;
    lx*) CPUS_PER_NODE=2
      ;;
    tc*) CPUS_PER_NODE=4
      ;;
    *) CPUS_PER_NODE=1
      ;;
  esac
  while test "$1"
  do case $1 in
    -np) NUM_PROCS=$2
      NUM_NODES=NUM_PROCS+CPUS_PER_NODE-1
      NUM_NODES=NUM_NODES/CPUS_PER_NODE
      return $NUM_NODES
      ;;
    *) shift
      ;;
  esac
  done
  return 1
}
function CalcProcs
{ # extract the "number of processes|task"
  while test "$1"
  do case $1 in
    -np) return $2
      ;;
    *) shift
      ;;
  esac
  done
  return 1
}
function CheckPath
{ # check the path to the executable
  while test "$1"
  do case $1 in
    -np) EXECFILE=$3
      if [[ ! -x $EXECFILE ]]
      then
        if [[ ! -x ../$EXECFILE ]]
        then
          print "Can not find executable!!!"
          return 1
        else
          cp ../$EXECFILE $EXECFILE
          return 0
        fi
      fi
      return 0
      ;;
    *) shift
      ;;
  esac
  done
  return 1
}
function PsubCmdStub
{ # initialize the common part of the " PsubCmd" string, ulgy global vars!
  # global "RunName" is assumed to be predefined
  CalcNodes "$@"
  NumNodes=$?
  CalcProcs "$@"
  NumProcs=$?
  typeset -L4 HOST=$(hostname)
  HOST=${HOST%%.*}              # remove possible ".llnl.gov"
  case $HOST in
    fros*) PsubCmd="psub -c frost,pbatch -b a_casc -nettype css0 -r $RunName"
      PsubCmd="$PsubCmd -ln $NumNodes -g $NumProcs"
      ;;
    blue*) PsubCmd="psub -c blue,pbatch -b a_casc -r $RunName"
      PsubCmd="$PsubCmd -ln $NumNodes -g $NumProcs"
      ;;
    tckk*) PsubCmd="psub -c tc2k,pbatch -b casc -r $RunName -ln $NumProcs"
      ;;
    peng*) PsubCmd="psub -c pengra,pbatch -b casc -r $RunName -ln $NumProcs"
      PsubCmd="$PsubCmd -standby"
      ;;
    gps*) PsubCmd="psub -c gps320 -b casc -r $RunName -cpn $NumProcs"
      ;;
    lx*) PsubCmd="psub -c lx -b casc -r $RunName -cpn $NumProcs"
      ;;
    tc*) PsubCmd="psub -c tera -b casc -r $RunName -cpn $NumProcs"
      ;;
    *) PsubCmd="psub -b casc -r $RunName -ln $NumProcs"
      ;;
  esac
}
function ExecuteJobs
{ # read job file line by line saving arguments
  WorkingDir=$1
  InputFile=$2
  if (( DebugMode > 0 ))
  then print "In function ParseJobFile: WorkingDir=$WorkingDir InputFile=$InputFile" ; fi
  integer ReturnFlag=0                  # error return flag
  integer BatchFlag=0                   # #Batch option detected flag 
  integer BatchCount=0                  # different numbering for #Batch option
  integer PrevPid=0
  typeset RunName
  SavePWD=$(pwd)
  cd $WorkingDir
  exec 3< $InputFile.jobs               # open *.jobs file for reading
  while read -u3 InputLine
  do
    if (( DebugMode > 0 )) ; then print $InputLine ; fi
    case $InputLine in
      \#Bat*|\#bat*|\#BAT*) BatchFlag=1
        BatchFile=""
        if (( DebugMode > 0 )) ; then print "Batch line" ; fi
        ;;
      \#End*|\#end*|\#END*) BatchFlag=0
        if (( DebugMode > 0 )) ; then print "Submit job" ; fi
        chmod +x $BatchFile
        PsubCmd="$PsubCmd -o $OutFile -e $ErrFile $(pwd)/$BatchFile"
        if (( NoRun == 0 )) ; then CmdReply=$($PsubCmd) ; fi
        PrevPid=$(print $CmdReply | cut -d \  -f 2)
        if (( DebugMode > 0 )) ; then print "PsubCmd=$PsubCmd $PrevPid" ; fi
        while [[ $(pstat | grep $PrevPid) != "" ]]
        do sleep $JobCheckInterval      # global, see runtest.sh
        done
        if (( DebugMode > 0 )) ; then print "Job finished $(date)" ; fi
        BatchFile=""
        ;;
      *mpirun*)
        if (( DebugMode > 0 )) ; then print "mpirun line" ; fi
        if (( DebugMode > 0 )) ; then print "BatchMode:$BatchMode BatchFlag:$BatchFlag" ; fi
	RunCmd=${InputLine#*mpirun} 
	RunCmd=$(echo $RunCmd | sed -e 's/[ \t]*>.*$//')
        OutFile=${InputLine#*'>'}
        OutFile=${OutFile## }
	ErrFile=$(echo $OutFile | sed -e 's/\.out\./.err./')
        RunName=${OutFile%%.out.*}
        CheckPath $RunCmd               # check path to executable
        if (( $? > 0 ))
        then
        cat >> $RunName.err <<- EOF
		Executable doesn't exist command: 
		$InputLine 
		EOF
          ReturnFlag=1
          break
        fi
        MpirunString $RunCmd            # construct "RunString"
        if (( BatchMode == 0 ))
        then
          if (( DebugMode > 0 )) ; then print "${RunString} > $OutFile 2> $ErrFile" ; fi
          ${RunString} > $OutFile 2> $ErrFile
        else
          if (( BatchFlag == 0 ))
          then BatchFile=$(echo $OutFile | sed -e 's/\.out\./.batch./')
            if (( DebugMode > 0 )) ; then print "RunName=$RunName BatchFile=$BatchFile" ; fi
            cat > $BatchFile <<- EOF 
		#!/bin/ksh
		cd $(pwd)
		${RunString}
		EOF
            chmod +x $BatchFile
            PsubCmdStub ${RunCmd}
            PsubCmd="$PsubCmd -o $OutFile -e $ErrFile $(pwd)/$BatchFile"
            if (( NoRun == 0 )) ; then CmdReply=$($PsubCmd) ; fi
            PrevPid=$(print $CmdReply | cut -d \  -f 2)
            if (( DebugMode > 0 )) ; then print "$PsubCmd $PrevPid" ; fi
            while [[ $(pstat | grep $PrevPid) != "" ]]
            do sleep $JobCheckInterval  # global, see runtest.sh
            done
            if (( DebugMode > 0 )) ; then print "Job finished $(date)" ; fi
          else                          # BatchFlag set
            if (( DebugMode > 0 )) ; then print "RunName=$RunName BatchFile=$BatchFile" ; fi
###         if [[ $BatchFile == "" ]]   # OSF1 doesn't support ksh93 convension
            if [[ $BatchFile = "" ]]
            then BatchFile=$InputFile.batch.$BatchCount
              BatchCount=BatchCount+1
              cat > $BatchFile <<- EOF
		#!/bin/ksh
		cd $(pwd)
		${RunString}
		EOF
            else
              cat >> $BatchFile <<- EOF
		${RunString}
		EOF
            fi
            PsubCmdStub ${RunCmd}       # construct a PsubCmd string
          fi                            # BatchFlag set
        fi                              # BatchMode set
        ;;
     ""|"\n") :
       if (( DebugMode > 0 )) ; then print "Blank line" ; fi
       ;;
     *#*) :
       if (( DebugMode > 0 )) ; then print "# line" ; fi
       ;; 
     *)
       print "Found something unexpected in $WorkingDir/$InputFile.jobs"
       print "In line: $InputLine"
       exit 1
       ;;
    esac
  done 
  exec 3<&-
  cd $SavePWD
  return $ReturnFlag
}
function ExecuteTest
{ #   
  if (( DebugMode > 0 )) ; then print "In function ExecuteTest" ; fi
  WorkingDir=$1
  InputFile=$2
  SavePWD=$(pwd)
  cd $WorkingDir
  if (( DebugMode > 0 )) ; then print "./$InputFile.sh  $InputFile.err " ; fi
  ./$InputFile.sh > $InputFile.err 2>&1
  cd $SavePWD
}
function PostProcess
{ #   
  if (( DebugMode > 0 )) ; then print "In function PostProcess" ; fi
  WorkingDir=$1
  InputFile=$2
  SavePWD=$(pwd)
  cd $WorkingDir
  if (( BatchMode == 0 ))
  then
    if [[ -f purify.log ]]
    then
      mv purify.log $InputFile.purify.log
      grep -i hypre_ $InputFile.purify.log >> $InputFile.err
    elif [[ -f insure.log ]]
    then
      mv insure.log $InputFile.insure.log
      grep -i hypre_ $InputFile.insure.log >> $InputFile.err
    fi
  fi
  cd $SavePWD
}

function StartCrunch
{ # process files
  ExecuteJobs "$@"
  if (( $? == 0 ))
  then ExecuteTest "$@"
  fi
  if (( $? == 0 ))
  then PostProcess "$@"
  fi
}
