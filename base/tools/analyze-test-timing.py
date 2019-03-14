#! /usr/bin/env python3

import logging
import sys

def analyze(fin):
	result = []

	for line in fin.readlines():
		line = line.strip()
		if line == "":
			continue

		parts = line.split()
		if len(parts) != 3:
			logging.error("skipping unexpected line '%s'", line)
			continue

		if parts[2] != "s":
			logging.error("unexpected time unit: '%s'", parts[2])
			continue

		name = parts[0]
		time = float(parts[1])

		result.append((name, time))

	return result

def report(result, threshold = 0.5):
	sum = 0
	fails = 0

	for name, time in result:
		sum += time

		if time > threshold:
			fails += 1
			print("FAIL: %s lasts too long (%f s)" % (name, time))

	return sum, fails

if __name__ == "__main__":
	result = analyze(sys.stdin)

	if len(sys.argv) > 1:
		threshold = float(sys.argv[1])
	else:
		threshold = 0.5

	sum, fails = report(result, threshold)

	print("total: %f s" % sum)

	if fails > 0:
		sys.exit(-1)

	sys.exit(0)
