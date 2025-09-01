#!/bin/bash
# Register iwasm-wrapper as interpreter for miscellaneous binary format
# Requires 'sudo' to run 

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
source $SCRIPT_DIR/../toolchains/wali.sh

# Whether to enable a permanent setup across reboots
permanent=0  # 0 for false, 1 for true

# Parse command-line options
while getopts ":p" opt; do
  case $opt in
    p) permanent=1;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      exit 1
      ;;
  esac
done

# Get the interp/aot conf from iwali.conf
mapfile -t lines < <(tail -n 2 "$SCRIPT_DIR/iwali.conf")
interp_conf="${lines[0]}"
aot_conf="${lines[1]}"

if [ $permanent -ne 0 ] ; then
  # Permanent binfmt registration
  echo "Setting up permanent binfmt registration across reboots"
  cp $SCRIPT_DIR/iwali.conf /etc/binfmt.d/
  systemctl restart systemd-binfmt
else
  # Temporary binfmt registration
  echo "Setting up temporary binfmt registration (this will not survive reboots.. run with -p option to allow that)"
  echo "$interp_conf" > /proc/sys/fs/binfmt_misc/register
  echo "$aot_conf" > /proc/sys/fs/binfmt_misc/register
fi
