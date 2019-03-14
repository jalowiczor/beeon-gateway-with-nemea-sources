<?xml version="1.0" encoding="utf-8"?>
<x:stylesheet xmlns:x="http://www.w3.org/1999/XSL/Transform"
		xmlns:e="http://exslt.org/common"
		extension-element-prefixes="e"
		version="1.0">

	<x:output method="text" encoding="utf-8" />
	<x:strip-space elements="*" />

	<x:param name="class.excludes" select="''" />
	<x:param name="default.level" select="'${logging.default.level}'" />

	<x:template match="/">
		<x:apply-templates select="doxygen/compounddef" />
	</x:template>

	<!-- Analyze BeeeOn::Loggable and its child classes -->
	<x:template match="compounddef[compoundname = 'BeeeOn::Loggable']">
		<x:apply-templates select="inheritancegraph" />
	</x:template>

	<!-- Ignore classes other then BeeeOn::Loggable -->
	<x:template match="compounddef" />

	<!-- Excludes template arguments from class name -->
	<x:template name="class-extract-name">
		<x:param name="label" select="label" />

		<x:choose>
			<x:when test="contains($label, '&lt;')">
				<x:value-of select="substring-before($label, '&lt;')" />
			</x:when>
			<x:otherwise>
				<x:value-of select="$label" />
			</x:otherwise>
		</x:choose>
	</x:template>

	<!-- Check whether the class is listed in $class.excludes -->
	<x:template name="class-is-excluded">
		<x:param name="label" select="label" />

		<x:variable name="class">
			<x:call-template name="class-extract-name" />
		</x:variable>

		<!-- Trick: replace "," by " ", and normalize spaces -->
		<x:variable name="remove.commas" select="translate($class.excludes, ',', ' ')" />
		<x:variable name="normalized" select="normalize-space($remove.commas)" />

		<x:choose>
			<!-- Trick: search for ' ' + class + ' ' (simulating regex specials: \< \>) -->
			<x:when test="contains(concat(' ', $normalized, ' '), concat(' ', $class, ' '))">
				<x:message>
					<x:text>excluded explicitly </x:text>
					<x:value-of select="translate($label, ' ', '')" />
				</x:message>
				<x:text>yes</x:text>
			</x:when>
			<x:otherwise>
				<x:text>no</x:text>
			</x:otherwise>
		</x:choose>
	</x:template>

	<!-- Detect the class is marked abstract or has pure-virtual methods -->
	<x:template name="class-is-abstract">
		<x:param name="label" select="label" />
		<x:param name="link" select="link" />

		<x:variable name="ref.id" select="$link/@refid" />
		<x:variable name="ref.file" select="concat($ref.id, '.xml')" />
		<x:variable name="ref" select="document($ref.file, /)/doxygen/compounddef" />
		<x:variable name="abstract" select="$ref/@abstract='yes'" />
		<x:variable name="has.pure.virtual"
			select="$ref/listofallmembers/member[@virt='pure-virtual']" />

		<x:variable name="result">
			<x:choose>
				<x:when test="$abstract = 'yes'">
					<x:text>yes</x:text>
				</x:when>
				<x:when test="$has.pure.virtual">
					<x:text>yes</x:text>
				</x:when>
				<x:otherwise>
					<x:text>no</x:text>
				</x:otherwise>
			</x:choose>
		</x:variable>

		<x:if test="$result = 'yes'">
			<x:message>
				<x:text>excluded abstract class </x:text>
				<x:value-of select="translate($label, ' ', '')" />
			</x:message>
		</x:if>

		<x:value-of select="$result" />
	</x:template>

	<!-- Select classes to generate logging configuration for -->
	<x:template match="inheritancegraph">
		<!-- Sort classes to find duplicates -->
		<x:variable name="sorted.nodes">
			<x:for-each select="node">
				<x:sort select="label" />
				<x:copy-of select="current()" />
			</x:for-each>
		</x:variable>

		<x:for-each select="e:node-set($sorted.nodes)/node">
			<!-- Prepare for check whether the previous class has equivalent name -->
			<x:variable name="prev" select="preceding-sibling::node[1]" />
			<x:variable name="prev.class">
				<x:call-template name="class-extract-name">
					<x:with-param name="label" select="$prev/label" />
				</x:call-template>
			</x:variable>

			<x:variable name="class">
				<x:call-template name="class-extract-name" />
			</x:variable>

			<x:variable name="is.duplicate">
				<x:if test="$prev.class = $class">
					<x:text>yes</x:text>
				</x:if>
			</x:variable>

			<x:variable name="is.excluded">
				<x:call-template name="class-is-excluded" />
			</x:variable>

			<x:variable name="is.abstract">
				<x:call-template name="class-is-abstract" />
			</x:variable>

			<x:variable name="skip"
				select="$is.duplicate = 'yes' or $is.abstract = 'yes' or $is.excluded = 'yes'" />

			<x:if test="not($skip)">
				<x:apply-templates select="." />
			</x:if>
		</x:for-each>
	</x:template>

	<!-- Generate logging configuration for a node -->
	<x:template match="node">
		<x:variable name="class">
			<x:call-template name="class-extract-name" />
		</x:variable>

		<x:text>loggers.</x:text>
		<x:value-of select="$class" />
		<x:text>.name = </x:text>
		<x:value-of select="$class" />
		<x:text>&#xA;</x:text>

		<x:text>loggers.</x:text>
		<x:value-of select="$class" />
		<x:text>.level = </x:text>
		<x:value-of select="$default.level" />
		<x:text>&#xA;</x:text>

		<x:text>&#xA;</x:text>
	</x:template>
</x:stylesheet>
