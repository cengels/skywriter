const MS_PER_DAY = 1000 * 60 * 60 * 24;

/** Determines whether a year is a leap year. */
function isLeapYear(year) {
    return year % 400 === 0 || (year % 100 !== 0 && year % 4 === 0);
}

/** Gets the number of days in a year. */
function daysInYear(year) {
    return isLeapYear(year) ? 366 : 365;
}

/** Gets the total number of days elapsed since the start of the year. */
function dayInYear(date) {
    let days = 0;

    for (let month = 0; month < date.getMonth(); month++) {
        days += daysInMonth(date.getFullYear(), month);
    }

    days += date.getDate();

    return days;
}

/** Gets the number of days in a month. */
function daysInMonth(year, month) {
    return new Date(year, month, 0).getDate();
}

/** Gets the number of days that have elapsed since a given date. */
function daysSince(date) {
    const now = new Date();
    let days = 0;

    if (date.getFullYear() !== now.getFullYear()) {
        days += dayInYear(date);

        for (let year = date.getFullYear() + 1; year < now.getFullYear(); year++) {
            days += daysInYear(year);
        }
    }

    for (let month = date.getMonth(); month < now.getMonth(); month++) {
        days += daysInMonth(date.getFullYear(), month);
    }

    days += now.getDate() - date.getDate();

    return days;
}

/** Converts a date to a relative time string. */
function relative(date) {
    const daysAgo = daysSince(date);

    function ago(value, singular) {
        if (value === 1) {
            return value + " " + singular + " ago";
        }

        return value + " " + singular + "s ago";
    }

    if (daysAgo === 0) {
        const msDifference = Date.now() - date.getTime();
        const secsDifference = msDifference / 1000;
        const minsDifference = secsDifference / 60;
        const hoursDifference = minsDifference / 60;

        if (minsDifference < 1) {
            return "now";
        } else if (hoursDifference < 1) {
            return ago(Math.floor(minsDifference), "minute");
        }

        return ago(Math.floor(hoursDifference), "hour");
    } else if (daysAgo === 1) {
        return "yesterday at " + date.toLocaleTimeString(Qt.locale());
    } else if (daysAgo <= 6) {
        return daysAgo + " days ago";
    } else if (daysAgo < 30) {
        return ago(Math.round(daysAgo / 7), "week");
    } else if (daysAgo < 365) {
        return ago(Math.round(daysAgo / 30), "month")
    }

    return ago(Math.round(daysAgo / 365), "year");
}
