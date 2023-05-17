$(document).ready(function() {
  $(".question_icon").hover(function() {
    $(this).closest(".question").children(".question_help_text").first().show();
  },function() {
    $(this).closest(".question").children(".question_help_text").first().hide();
  });

  $(".question_help_text").hide();
});
