#! /bin/sh

echo rm -f ../amix-modified.tgz && \
	rm -f ../amix-modified.tgz && \
	echo cd .. && \
	cd .. && \
	echo tar czf amix-modified.tgz amix-modified/ && \
	tar czf amix-modified.tgz amix-modified/ && \
	echo Zrobione. && ls -l amix-modified.tgz && exit 0
echo 'Skrypt zakonczyl sie niepowodzeniem.'
exit 1
