program main
  use baobzi
  implicit none
  real(kind=c_double) :: center(2), half_length(2), tol
  real(kind=c_double) :: x(2)
  real(kind=c_double), target :: scale_factor
  character(len=64) :: fname
  type(c_ptr) :: func_approx
  type(baobzi_input_t) :: input

  input%func = c_funloc(testfun)
  input%data = c_loc(scale_factor)
  input%dim = 2
  input%order = 6
  input%tol = 1E-8

  scale_factor = 1.0

  center(:) = 0.0
  half_length(:) = 1.0

  x(:) = 0.25

  fname = trim(adjustl('fortran.baobzi'))//char(0)

  func_approx = baobzi_init(input, center, half_length)
  print *, baobzi_eval(func_approx, x) - testfun(x, scale_factor)

  call baobzi_save(func_approx, fname)
  func_approx = baobzi_free(func_approx)

  func_approx = baobzi_restore(fname)
  print *, baobzi_eval(func_approx, x) - testfun(x, scale_factor)

contains
  function testfun (x, data) bind(c) result(y)
    use, intrinsic :: iso_c_binding
    implicit none
    real(kind=c_double), dimension(*) :: x
    real(kind=c_double) :: data
    real(kind=c_double) :: y

    y = data * x(1) * x(2)
  end function testfun

end program main
