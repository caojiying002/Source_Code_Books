
# Check that compilation tools are installed

#...


# Assume that $user contains the user name, $cvsserver contains

# the CVS server name and /path/to/repository is the location

# of your project's repository on the CVS server

CVS="cvs -d :pserver:$user@$cvsserver:/path/to/repository"
$CVS login

# Check-out the kernel

$CVS checkout kernel

# Build the kernel

cd kernel
make mrproper
#Get the .config file for your platform

cp arch/your-arch/configs/your_platform_defconfig .config
make oldconfig
make -j5 bzImage # Accelerate by spawning 5 instances of 'make'
if [ $? != 0 ]
then
echo "Error building Kernel. Bailing out.."
exit 1
fi

# Copy the kernel image to a target directory

cp arch/x86/boot/bzImage /path/to/target_directory/productname.kernel

# Build modules and install them in an appropriate directory

make modules
if [ $? != 0 ]
then
echo "Error building modules. Bailing.."
exit 2
fi

export INSTALL_MOD_PATH="$TARGET_DIRECTORY/modules"
make modules_install

# Rebuild after forcing generation of identical MD5 sums and

# package the resulting checksum information.

#...


# Generate a source patch from the base starting point, assuming

# that KERNELBASE is the CVS tag for the vanilla kernel

cvs rdiff -u -r KERNELBASE kernel > patch.kernel

# Generate a changelog using "cvs log"

#...


# Package everything nicely into a tar ball

#...

