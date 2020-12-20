const VOWELS = ['a', 'e', 'i', 'o', 'u', 'h'];

/** Converts a date to a relative time string. */
function relative(date) {
    const today = new Date().setHours(0, 0, 0, 0);
    const msDifference = Date.now() - date.getTime();
    const secsDifference = msDifference / 1000;
    const minsDifference = secsDifference / 60;
    const hoursDifference = minsDifference / 60;

    // We calculate the days slightly differently.
    // For instance, we don't want to display "16 hours ago"
    // when it is May 3, 12 pm and the file was last saved on May 2, 8 pm.
    // If the date differs, we never want to display "n hours ago".
    // To accomplish this, we use the start of the day in POSIX time here
    // instead of the now() value itself.
    const daysDifference = (today - date.getTime()) / 1000 / 60 / 60 / 24;

    function ago(value, singular) {
        if (value === 1) {
            const isVowel = VOWELS.some(vowel => singular.startsWith(vowel));
            const article = isVowel ? 'an' : 'a';
            return `${article} ${singular} ago`;
        }

        return `${value} ${singular}s ago`;
    }

    if (minsDifference < 1) {
        return "now";
    } else if (hoursDifference < 1) {
        return ago(Math.floor(minsDifference), "minute");
    } else if (daysDifference <= 0) {
        // daysDifference will be negative if `date` is today.
        return ago(Math.floor(hoursDifference), "hour");
    } else if (daysDifference < 1) {
        return "yesterday at " + date.toLocaleTimeString(Qt.locale());
    }
    
    const daysAgo = Math.floor(daysDifference + 1);

    if (daysAgo < 7) {
        return daysAgo + " days ago";
    } else if (daysAgo < 30) {
        return ago(Math.round(daysAgo / 7), "week");
    }

    const monthsAgo = Math.round(daysAgo / 30);

    if (monthsAgo < 12) {
        return ago(monthsAgo, "month");
    }

    return ago(Math.round(daysAgo / 365), "year");
}
