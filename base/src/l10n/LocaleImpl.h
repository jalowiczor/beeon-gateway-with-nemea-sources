#pragma once

namespace BeeeOn {

/**
 * Abstract representation of locale. This makes us independent
 * on the target locale implementation.
 */
class LocaleImpl {
public:
	virtual ~LocaleImpl();

	/**
	 * Returns language code of the locale.
	 */
	virtual std::string language() const = 0;

	/**
	 * Returns country code of the locale.
	 */
	virtual std::string country() const = 0;

	/**
	 * Name to display to user.
	 *
	 * It displays its name via its locale. Thus if the locale is
	 * en_US, you would get English (USA). If the locale is cs_CZ,
	 * the result would be Čeština (Česká republika).
	 */
	virtual std::string displayName() const = 0;

	/**
	 * String representation of the locale.
	 * It is usually a code like en_US.
	 */
	virtual std::string toString() const = 0;
};

class SystemLocaleImpl : public LocaleImpl {
public:
	std::string language() const override;
	std::string country() const override;
	std::string displayName() const override;
	std::string toString() const override;
};

}
