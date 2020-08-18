set breakpoint pending on
set confirm off
file ./negac
break negatron::Err::report
commands
	where
end
break negatron::InternalError::InternalError
commands
	where
end

define p2
  set args p2_tests/$arg0.nega -p --
  run
end
