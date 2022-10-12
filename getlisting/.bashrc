function icpc-next()
{
	for f in $(find ~/icpc/contests -type d | sort -V)
	do
		if [ ! -f ${f}/done ]
		then
			cd ${f}
			return
		fi
	done
}

