package rickroll

import (
	"testing"
)

func TestIsRickRoll(t *testing.T) {
	var tests = []struct {
		value string
		wants SafetyRating
	}{
		{"rick roll", Danger},
		{"never gonna give you up", Danger},
		{"Rick Astley", Danger},
		{"He who must not be named", Safe},
	}

	for _, tt := range tests {
		rating := isRickRoll([]byte(tt.value))
		if rating != tt.wants {
			t.Errorf("isRickRoll(%s) = %d; want %d", tt.value, rating, tt.wants)
		}
	}
}
