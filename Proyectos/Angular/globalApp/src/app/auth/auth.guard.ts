import { Injectable } from '@angular/core';
import { CanActivate, CanActivateChild, CanLoad, Route, UrlSegment,
          ActivatedRouteSnapshot, RouterStateSnapshot, UrlTree, Router, NavigationExtras } from '@angular/router';
import { Observable } from 'rxjs';

import { AuthService } from './auth.service';

@Injectable({
  providedIn: 'root'
})
export class AuthGuard implements CanActivate, CanActivateChild, CanLoad {
  constructor(private authService: AuthService, private router: Router) {}

  /*searchData(state: RouterStateSnapshot, dataKey: string): string | null {
    if (state.root.data && state.root.data[dataKey]) {
      return state.root.data[dataKey];
    }
    let child: ActivatedRouteSnapshot | null;
    child = state.root.firstChild;
    while (child != null) {
        if (child.data && child.data[dataKey]) {
            return child.data[dataKey];
        }
        child = child.firstChild;
    }
    return null;
  }*/

  canActivate(
    route: ActivatedRouteSnapshot,
    state: RouterStateSnapshot): boolean {
      /*const url: string = state.url;
      return this.checkLogin(url);*/
      const currentUser = this.authService.currentUserValue;
      if (currentUser) {
        // console.log('route.data: %o', this.searchData(state, 'roles'));
        // console.log('Roles: %o', route.data.roles);
        // check if route is restricted by role
        if (route.data.roles && route.data.roles.indexOf(currentUser.rol) === -1) {
            // role not authorised so redirect to home page
            this.router.navigate(['/']);
            return false;
        }

        // authorised so return true
        return true;
      }

      // not logged in so redirect to login page with the return url
      this.router.navigate(['/login'], { queryParams: { returnUrl: state.url } });
      return false;
  }

  canActivateChild(
    route: ActivatedRouteSnapshot,
    state: RouterStateSnapshot): boolean {
      return this.canActivate(route, state);
  }

  canLoad(route: Route): boolean {
    // era let no const
    const url = '/${route.path}';

    return this.checkLogin(url);
  }

  checkLogin(url: string): boolean {
    if (this.authService.isLoggedIn) { return true; }

    // Store the attempted URL for redirecting
    this.authService.redirectUrl = url;

    // Create a dummy session id (era let)
    const sessionID = 123456789;

    // Set our navigation extras object (era let)
    // that contains our global query params and fragment
    const navigationExtras: NavigationExtras = {
      queryParams: { 'session_id': sessionID },
      fragment: 'anchor'
    };

    // Navigate to the login page with extras
    this.router.navigate(['/login'], navigationExtras);
    return false;
  }
}
